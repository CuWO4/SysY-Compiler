#include "../include/ast.h"
#include "../include/koopa.h"
#include "../include/koopa_trans.h"
#include "../include/nesting_info.h"
#include "../include/value_manager.h"
#include "../include/name.h"
#include "../include/loop_tag.h"

#include <string>

namespace ast {

koopa_trans::Blocks *BinaryExpr::to_koopa() const {
    // TODO refactor
    auto res = new koopa_trans::Blocks;

    koopa_trans::Blocks *lv_stmts;
    if (op != op::ASSIGN) lv_stmts = lv->to_koopa();
    auto rv_stmts = rv->to_koopa();

    /**
     * @param has_pushed  whether lv_stmts & rv_stmts have been pushed into res
     */
    auto generator = [&] (
        koopa::Op op, 
        koopa::Value *lv_koopa, 
        koopa::Value *rv_koopa, 
        bool has_pushed = false
    ) -> koopa::Value * {
        if (lv_koopa->is_const && rv_koopa->is_const) {
            return value_manager.new_const(
                koopa::op::op_func[op](lv_koopa->val, rv_koopa->val)
            );
        }

        if (!has_pushed) {
            *res += *lv_stmts;
            *res += *rv_stmts;
        }

        auto new_id = value_manager.new_id(
            koopa::id_type::LocalId,
            new koopa::Int,
            new_id_name(),
            new NestingInfo()
        );

        *res += new koopa::SymbolDef(
            new_id,
            new koopa::Expr(
                op, lv_koopa, rv_koopa
            )
        );

        return new_id;
    };

    auto build_short_circuit_eval_stmts = [&] (BinaryOp op) -> koopa::Value * {
    /*
     *  LOGIC_AND:
     *      int result = lv != 0;
     *      if (result) 
     *          result = rv != 0;
     *
     *  LOGIC_OR:
     *      int result = lv != 0;
     *      if (!result)
     *          result = rv != 0;
     *
     *
     *             +
     *      // alloc result
     *      // result = lv != 0
     *      // if run on? -----------*
     *                               |
     *                            T  |
     *      // result = rv != 0 <----+
     *                |              |
     *                V            F |
     *      // last_val = result <---*
     */
        if (op != op::LOGIC_AND && op != op::LOGIC_OR) {
            const char *binary_op_name[] = {
                "||", "&&", "==", "!=", "<", ">", "<=", ">=", 
                "+", "-", "*", "/", "%", ",", "="
            };
            throw std::string("trying to build short circuit evaluation statements for `") 
                + binary_op_name[op] + '`';
        }

        auto elv = lv_stmts->get_last_val();
        auto erv = rv_stmts->get_last_val();

        if (!rv->has_side_effect()) {
            if (!elv->is_const) *res += *lv_stmts;
            if (!erv->is_const) *res += *rv_stmts;
            auto tmpa = generator(koopa::op::NE, elv, value_manager.new_const(0), true);
            auto tmpb = generator(koopa::op::NE, erv, value_manager.new_const(0), true);
            return generator(
                op == op::LOGIC_AND
                    ? koopa::op::AND
                    : koopa::op::OR, 
                tmpa, tmpb,
                true
            );
        }

        if (elv->is_const && erv->is_const) {
            return value_manager.new_const(
                op == op::LOGIC_AND
                    ? elv->val && erv->val
                    : elv->val || erv->val
            );
        }
        if (elv->is_const) {
            if (op == op::LOGIC_AND && elv->val == 0) return value_manager.new_const(0);
            if (op == op::LOGIC_OR && elv->val != 0) return value_manager.new_const(1);
        }

        auto res_addr = value_manager.new_id(
            koopa::id_type::LocalId, 
            new koopa::Pointer(new koopa::Int), 
            new_id_name()
        );
        auto blocks = new koopa_trans::Blocks;

        auto then_block = new koopa_trans::Blocks;
        auto end_block = new koopa_trans::Blocks;

        *blocks += new koopa::SymbolDef(res_addr, new koopa::MemoryDecl(new koopa::Int));

        koopa::Value *bool_lv;
        if (elv->is_const) {
            bool_lv = value_manager.new_const(elv->val != 0);
        }
        else {
            bool_lv = value_manager.new_id(
                koopa::id_type::LocalId, 
                new koopa::Int, 
                new_id_name()
            );
            *blocks += *lv_stmts;
            *blocks += new koopa::SymbolDef(
                static_cast<koopa::Id *>(bool_lv),
                new koopa::Expr(
                    koopa::op::NE,
                    elv,
                    value_manager.new_const(0)
                )
            );
        }
        *blocks += new koopa::StoreValue(bool_lv, res_addr);

        if (op == op::LOGIC_AND) {
            *blocks += new koopa::Branch(
                bool_lv, 
                then_block->get_begin_block_id(), 
                end_block->get_begin_block_id()
            );
        }
        else if (op == op::LOGIC_OR) {
            *blocks += new koopa::Branch(
                bool_lv, 
                end_block->get_begin_block_id(), 
                then_block->get_begin_block_id()
            );
        }

        koopa::Value *bool_rv;
        if (erv->is_const) {
            bool_rv = value_manager.new_const(erv->val != 0);
        }
        else {
            bool_rv = value_manager.new_id(
                koopa::id_type::LocalId, 
                new koopa::Int, 
                new_id_name()
            );
            *then_block += *rv_stmts;
            *then_block += new koopa::SymbolDef(
                static_cast<koopa::Id *>(bool_rv),
                new koopa::Expr(
                    koopa::op::NE,
                    elv,
                    value_manager.new_const(0)
                )
            );
        }
        *then_block += new koopa::StoreValue(bool_rv, res_addr);
        *then_block += new koopa::Jump(end_block->get_begin_block_id());

        *blocks += then_block->to_raw_blocks();
        *blocks += end_block->to_raw_blocks();

        *res += *blocks;

        auto res_id = value_manager.new_id(koopa::id_type::LocalId, new koopa::Int, new_id_name());
        *res += new koopa::SymbolDef(res_id, new koopa::Load(res_addr));
        
        return res_id;
    };

    switch (op) {
        case op::LOGIC_AND: case op::LOGIC_OR: res->set_last_val(
			build_short_circuit_eval_stmts(op)
		);
		break;
        case op::EQ: res->set_last_val(
			generator(koopa::op::EQ, lv_stmts->get_last_val(), rv_stmts->get_last_val())
		);
		break;
        case op::NEQ: res->set_last_val(
			 generator(koopa::op::NE, lv_stmts->get_last_val(), rv_stmts->get_last_val())
		);
		break;
        case op::LT: res->set_last_val(
			generator(koopa::op::LT, lv_stmts->get_last_val(), rv_stmts->get_last_val())
		);
		break;
        case op::GT: res->set_last_val(
			generator(koopa::op::GT, lv_stmts->get_last_val(), rv_stmts->get_last_val())
		);
		break;
        case op::LEQ: res->set_last_val(
			generator(koopa::op::LE, lv_stmts->get_last_val(), rv_stmts->get_last_val())
		);
		break;
        case op::GEQ: res->set_last_val(
			generator(koopa::op::GE, lv_stmts->get_last_val(), rv_stmts->get_last_val())
		);
		break;
        case op::ADD: res->set_last_val(
			generator(koopa::op::ADD, lv_stmts->get_last_val(), rv_stmts->get_last_val())
		);
		break;
        case op::SUB: res->set_last_val(
			generator(koopa::op::SUB, lv_stmts->get_last_val(), rv_stmts->get_last_val())
		);
		break;
        case op::MUL: res->set_last_val(
			generator(koopa::op::MUL, lv_stmts->get_last_val(), rv_stmts->get_last_val())
		);
		break;
        case op::DIV: res->set_last_val(
			generator(koopa::op::DIV, lv_stmts->get_last_val(), rv_stmts->get_last_val())
		);
		break;
        case op::MOD: res->set_last_val(
			generator(koopa::op::MOD, lv_stmts->get_last_val(), rv_stmts->get_last_val())
		);
		break;
        case op::COMMA: { 
            if (lv->has_side_effect()) *res += *lv_stmts;
            if (rv_stmts->get_last_val()->is_const) {
                res->set_last_val( rv_stmts->get_last_val() );
            }
            else {
                *res += *rv_stmts;
                res->set_last_val( rv_stmts->get_last_val() );
            }
            break;
        }
        case op::ASSIGN: {
            if (typeid(*lv) != typeid(ast::Id)) {
                throw "try to declare a non-id";
            }

            auto id = static_cast<ast::Id *>(lv);
            *res += *rv_stmts;

            auto id_koopa = value_manager.get_id('@' + id->lit, id->nesting_info);

            if (id_koopa == nullptr) {
                throw "undeclared identifier `" + id->lit + '`';
            }
            if (id_koopa->is_const) {
                throw "assigning to a const identifier `" + id->lit + '`';
            }

            *res += new koopa::StoreValue(
                res->get_last_val(),
                value_manager.get_id('@' + id->lit, id->nesting_info)
            );

            res->set_last_val( id_koopa );

            break;
        }
    }

    return res;
}

koopa_trans::Blocks *UnaryExpr::to_koopa() const {
    auto res = new koopa_trans::Blocks;

    auto lv_stmts = lv->to_koopa();

    auto generator = [&](koopa::Op op) -> koopa::Value * {
        if (lv_stmts->get_last_val()->is_const) {
            return value_manager.new_const(koopa::op::op_func[op](0, lv_stmts->get_last_val()->val));
        }

        *res += *lv_stmts;

        if (op == koopa::op::ADD) return lv_stmts->get_last_val();

        koopa::Id *new_id;

        new_id = value_manager.new_id(
            koopa::id_type::LocalId,
            new koopa::Int,
            new_id_name(),
            new NestingInfo()
        );

        *res += new koopa::SymbolDef(
            new_id,
            new koopa::Expr(
                op,
                value_manager.new_const(0),
                lv_stmts->get_last_val()
            )
        );

        return new_id;
    };

    switch (op) {
        case op::POS: res->set_last_val( generator(koopa::op::ADD) ); break;
        case op::NOT: res->set_last_val( generator(koopa::op::EQ) ); break;
        case op::NEG: res->set_last_val( generator(koopa::op::SUB) );
    }

    return res;
}

koopa_trans::Blocks *Number::to_koopa() const {
    auto res = new koopa_trans::Blocks;
    res->set_last_val( value_manager.new_const(val) );
    return res;
}

koopa_trans::Blocks *Id::to_koopa() const {
    auto res = new koopa_trans::Blocks;

    auto id_koopa = value_manager.get_id('@' + lit, nesting_info);

    if (id_koopa == nullptr) {
        throw "undeclared identifier `" + lit + '`';
    }

    if (id_koopa->is_const) {

        res->set_last_val( value_manager.new_const(id_koopa->val) );
        return res;

    }
    else {

        auto new_id = value_manager.new_id(
            koopa::id_type::LocalId,
            static_cast<koopa::Pointer *>(id_koopa->type)->pointed_type, //!? re-free bug
            new_id_name(),
            new NestingInfo()
        );

        *res += new koopa::SymbolDef(
            new_id,
            new koopa::Load(id_koopa)
        );

        res->set_last_val( new_id );

        return res;

    }
}

bool is_consistent(koopa::FuncType *func_type, std::vector<Expr *> actual_params) {
    if (func_type->arg_types.size() != actual_params.size()) return false;
    
    // TODO  since ast::Expr currently only has int type, the type is not checked

    return true;
}

koopa_trans::Blocks *FuncCall::to_koopa() const {
    auto res = new koopa_trans::Blocks();

    auto func_id_koopa = value_manager.get_func_id('@' + func_id->lit, func_id->nesting_info);

    if (func_id_koopa == nullptr) {
        throw "call of function `" + func_id->lit + "` undeclared";
    }

    if (!is_consistent(
            dynamic_cast<koopa::FuncType *>(func_id_koopa->type), 
            actual_params
        )) {
        throw "calling function `" + func_id->lit + "` with mismatched actual arguments";
    }

    auto actual_params_koopa = std::vector<koopa::Value *>();
    actual_params_koopa.reserve(actual_params.size());
    for (auto actual_param: actual_params) {
        auto actual_param_koopa = actual_param->to_koopa();
        *res += *actual_param_koopa;
        actual_params_koopa.push_back(actual_param_koopa->get_last_val());
    }

    auto ret_type_koopa = dynamic_cast<koopa::FuncType *>(func_id_koopa->type)->ret_type;
    if (ret_type_koopa->get_type_id() == koopa::type::Void) {
        *res += new koopa::FuncCall(func_id_koopa, actual_params_koopa);
        res->throw_last_val();
    }
    else {
        auto res_id = value_manager.new_id(
            koopa::id_type::LocalId,
            ret_type_koopa,
            new_id_name()
        );
        *res += new koopa::SymbolDef(
            res_id,
            new koopa::FuncCall(func_id_koopa, actual_params_koopa)
        );
        res->set_last_val( res_id );
    }

    return res;
}

koopa_trans::GlobalStmts *GlobalVarDef::to_koopa() const {
    if (value_manager.is_id_declared('@' + id->lit, id->nesting_info)) {
        throw '`' + id->lit + "` redefined";
    }

    auto stmts = new koopa_trans::GlobalStmts();
    
    if (decl_type == decl_type::ConstDecl) {
        if (!has_init) {
            throw "no initiator for const variable `" + id->lit + '`';
        }

        auto rval_koopa = init->to_koopa();

        if (!rval_koopa->get_last_val()->is_const) {
            throw "initiating const variable `" + id->lit + "` with a non-const value";
        }

        value_manager.new_id(
            koopa::id_type::GlobalId,
            type->to_koopa(),
            '@' + id->lit,
            id->nesting_info,
            true,
            rval_koopa->get_last_val()->val
        );
    }
    else { /* decl_type != decl_type::ConstDecl */
        if (has_init) {
            auto rval_koopa = init->to_koopa();

            if (!rval_koopa->get_last_val()->is_const) {
                throw "initiating global variable `" 
                    + id->lit + "` with a non-const value";
            }

            *stmts += new koopa::GlobalSymbolDef(
                value_manager.new_id(
                    koopa::id_type::GlobalId,
                    new koopa::Pointer(
                        type->to_koopa()
                    ),
                    '@' + id->lit,
                    id->nesting_info
                ),
                new koopa::GlobalMemoryDecl(
                    type->to_koopa(),
                    new koopa::ConstInitializer(rval_koopa->get_last_val()->val)
                )
            );
        }
        else { /* !has_init */
            *stmts += new koopa::GlobalSymbolDef(
                value_manager.new_id(
                    koopa::id_type::GlobalId,
                    new koopa::Pointer(
                        type->to_koopa()
                    ),
                    '@' + id->lit,
                    id->nesting_info
                ),
                new koopa::GlobalMemoryDecl(
                    type->to_koopa(),
                    new koopa::ConstInitializer(0)
                )
            );
        }
    } /* decl_type ?= decl_type::ConstDecl */

    return stmts;
}

koopa_trans::GlobalStmts *GlobalVarDecl::to_koopa() const {
    auto stmts = new koopa_trans::GlobalStmts();

    for (auto var_def: var_defs) {
        *stmts += *var_def->to_koopa();
    }

    return stmts;
}

koopa_trans::Blocks *VarDef::to_koopa() const {
    if (value_manager.is_id_declared('@' + id->lit, id->nesting_info)) {
        throw '`' + id->lit + "` redefined";
    }
    
    auto stmts = new koopa_trans::Blocks();
    
    if (decl_type == decl_type::ConstDecl) {
        if (!has_init) {
            throw "no initiator for const variable `" + id->lit + '`';
        }

        auto rval_koopa = init->to_koopa();

        if (!rval_koopa->get_last_val()->is_const) {
            throw "initiating const variable `" 
                + id->lit + "` with a non-const value";
        }

        value_manager.new_id(
            koopa::id_type::LocalId,
            type->to_koopa(),
            '@' + id->lit,
            id->nesting_info,
            true,
            rval_koopa->get_last_val()->val
        );
    }
    else { /* decl_type != decl_type::ConstDecl */
        *stmts += new koopa::SymbolDef(
            value_manager.new_id(
                koopa::id_type::LocalId,
                new koopa::Pointer(
                    type->to_koopa()
                ),
                '@' + id->lit,
                id->nesting_info
            ),
            new koopa::MemoryDecl(type->to_koopa())
        );

        if (has_init) {
            auto rval_koopa = init->to_koopa();
            *stmts += *rval_koopa;
            *stmts += new koopa::StoreValue(
                rval_koopa->get_last_val(),
                value_manager.get_id('@' + id->lit, id->nesting_info)
            );
        }
    }

    return stmts;
}

koopa_trans::Blocks *VarDecl::to_koopa() const {
    auto stmts = new koopa_trans::Blocks();

    for (auto var_def: var_defs) {
        *stmts += *var_def->to_koopa();
    }

    return stmts;
}

koopa_trans::Blocks *Return::to_koopa() const {
    if (return_type == return_type::NotHasRetVal) {
        auto res = new koopa_trans::Blocks();
        *res += new koopa::Return();
        return res;
    }

    auto res = ret_val->to_koopa();

    *res += new koopa::Return( res->get_last_val() );

    return res;
}

koopa_trans::Blocks *If::to_koopa() const {
    /*
     *             +
     *         cond_stmts ----*
     *                        |
     *                      T |
     *   *---- then_block <---+
     *   |                    |
     *   |                  F |
     *   +---- else_block <---*
     *   |     
     *   |
     *   *----> end_block
     *
     *
     *             +
     *         cond_stmts ----*
     *                        |
     *                      T |
     *   *---- then_block <---+
     *   |                    |
     *   |                  F |
     *   *----> end_block <---*
     */
    auto res = new koopa_trans::Blocks;

    if (has_else_stmt) {
        *res += *cond->to_koopa();

        auto cond_koopa = res->get_last_val();

        auto then_blocks = then_stmt->to_koopa()->to_raw_blocks();
        auto else_blocks = else_stmt->to_koopa()->to_raw_blocks();
        auto end_block = new koopa::Block(
            value_manager.new_id(
                koopa::id_type::BlockLabel,
                new koopa::Label,   
                new_block_name(), 
                new NestingInfo()
            ),
            {}
        );

        *res += new koopa::Branch(
            cond_koopa,
            then_blocks.front()->id,
            else_blocks.front()->id
        );

        *then_blocks.back() += new koopa::Jump(end_block->id);
        *else_blocks.back() += new koopa::Jump(end_block->id);

        *res += then_blocks;
        *res += else_blocks;
        *res += end_block;
    }
    else {  /* !has_else_stmt */
        *res += *cond->to_koopa();

        auto cond_koopa = res->get_last_val();

        auto then_blocks = then_stmt->to_koopa()->to_raw_blocks();
        auto end_block = new koopa::Block(
            value_manager.new_id(
                koopa::id_type::BlockLabel,
                new koopa::Label,   
                new_block_name(), 
                new NestingInfo()
            ),
            {}
        );

        *res += new koopa::Branch(
            cond_koopa,
            then_blocks.front()->id,
            end_block->id
        );

        *then_blocks.back() += new koopa::Jump(end_block->id);

        *res += then_blocks;
        *res += end_block;
    }

    return res;
}

koopa_trans::Blocks *While::to_koopa() const {
    /*
     *              |
     *              V
     *    *---- while_entry <----*
     *    |         ^            |
     *    | T       | continue   |
     *    +---> while_body ------*
     *    |         | break
     *    | F       V
     *    *----> while_end
     */
    auto res = new koopa_trans::Blocks;

    auto while_entry = new koopa_trans::Blocks;
    auto while_body = new koopa_trans::Blocks;
    auto while_end = new koopa_trans::Blocks;

    loop_tag_manager.push(
        LoopTag(
            while_entry->get_begin_block_id(), 
            while_end->get_begin_block_id()
        )
    );

    *res += new koopa::Jump(while_entry->get_begin_block_id());

    auto cond_koopa = cond->to_koopa();
    *while_entry += *cond_koopa;
    *while_entry += new koopa::Branch(
        cond_koopa->get_last_val(), 
        while_body->get_begin_block_id(), 
        while_end->get_begin_block_id()
    );

    auto body_koopa = body->to_koopa();
    *while_body += *body_koopa;
    *while_body += new koopa::Jump(while_entry->get_begin_block_id());

    *res += while_entry->to_raw_blocks();
    *res += while_body->to_raw_blocks();
    *res += while_end->to_raw_blocks();

    loop_tag_manager.pop();

    return res;
}

koopa_trans::Blocks *For::to_koopa() const {
    /*
     *              +
     *           for_init
     *              |
     *              V
     *    *----- for_entry <--------*
     *    |                         |
     *    |  T             break    |
     *    +----> for_body -------*  |
     *    |       |  | continue  |  |
     *    |       V  V           |  |
     *    |      for_iter -------+--*
     *    |  F                   |
     *    *----> for_end <-------*
     */
     auto res = new koopa_trans::Blocks;

    auto for_entry = new koopa_trans::Blocks;
    auto for_body = new koopa_trans::Blocks;
    auto for_iter = new koopa_trans::Blocks;
    auto for_end = new koopa_trans::Blocks;

    loop_tag_manager.push(
        LoopTag(
            for_iter->get_begin_block_id(), 
            for_end->get_begin_block_id()
        )
    );

    *res += *init_stmt->to_koopa();
    *res += new koopa::Jump(for_entry->get_begin_block_id());

    auto cond_koopa = cond->to_koopa();
    *for_entry += *cond_koopa;
    *for_entry += new koopa::Branch(
        cond_koopa->get_last_val(), 
        for_body->get_begin_block_id(), 
        for_end->get_begin_block_id()
    );

    auto body_koopa = body->to_koopa();
    *for_body += *body_koopa;
    *for_body += new koopa::Jump(for_iter->get_begin_block_id());

    auto iter_koopa = iter_stmt->to_koopa();
    *for_iter += *iter_koopa;
    *for_iter += new koopa::Jump(for_entry->get_begin_block_id());

    *res += for_entry->to_raw_blocks();
    *res += for_body->to_raw_blocks();
    *res += for_iter->to_raw_blocks();
    *res += for_end->to_raw_blocks();

    loop_tag_manager.pop();

     return res;
}

koopa_trans::Blocks *Continue::to_koopa() const {
    if (loop_tag_manager.empty()) {
        throw "not using `continue` in loop";
    }
    return new koopa_trans::Blocks(
        { new koopa::Jump(loop_tag_manager.top().continue_target) }
    );
}

koopa_trans::Blocks *Break::to_koopa() const {
    if (loop_tag_manager.empty()) {
        throw "not using `break` in loop";
    }
    return new koopa_trans::Blocks(
        { new koopa::Jump(loop_tag_manager.top().break_target) }
    );
}

static void trim_redundant_stmts_after_end_stmt(
    std::vector<koopa::Stmt *> &stmts, 
    koopa::Type *ret_type
) {
    bool exist_end_stmt = false;

    for (auto it = stmts.begin(); it != stmts.end(); it = std::next(it)) {
        if ((*it)->is_end_stmt()) {
            exist_end_stmt = true;
            stmts.erase(std::next(it), stmts.end());
            break;
        }
    }

    if (!exist_end_stmt) {
        koopa::Return *ret_stmt = nullptr;
        if (ret_type->get_type_id() == koopa::type::Int) {
            ret_stmt = new koopa::Return(value_manager.new_const(0));
        }
        else if (ret_type->get_type_id() == koopa::type::Void) {
            ret_stmt = new koopa::Return();
        }
        else {
            // TODO
        }
        stmts.push_back(ret_stmt);
    }
}

static void trim_redundant_stmts_after_end_stmt(
    koopa_trans::Blocks *blocks, 
    koopa::Type *ret_type
) {
    trim_redundant_stmts_after_end_stmt(blocks->active_stmts, ret_type);
    for (auto block: blocks->blocks) {
        trim_redundant_stmts_after_end_stmt(block->stmts, ret_type);
    }
}

static void add_pred_succ(koopa_trans::Blocks *blocks) {
    //TODO
}

koopa_trans::Blocks *Block::to_koopa() const {
    auto res = new koopa_trans::Blocks;

    for (auto stmt: stmts) {
        *res += *stmt->to_koopa();
    }

    return res;
}

koopa::Type *Int::to_koopa() const {
    return new koopa::Int;
}

koopa::Type *Void::to_koopa() const {
    return new koopa::Void;
}

koopa_trans::GlobalStmts *FuncDef::to_koopa() const {
    if (value_manager.get_id('@' + id->lit, id->nesting_info) != nullptr) {
        throw "`" + id->lit + "` redefined";
    }

    value_manager.enter_func(id->lit);

    auto param_types = std::vector<koopa::Type *>();
    auto param_ids = std::vector<koopa::Id *>();

    value_manager.enter_formal_params();
    param_types.reserve(params.size());
    param_ids.reserve(params.size());
    for (auto param: params) {
        auto param_type_koopa = std::get<0>(*param)->to_koopa();
        auto param_id_koopa = value_manager.new_id(
            koopa::id_type::LocalId,
            param_type_koopa, 
            '@' + std::get<1>(*param)->lit,
            std::get<1>(*param)->nesting_info
        );
        param_types.push_back(param_type_koopa);
        param_ids.push_back(param_id_koopa);
    }
    value_manager.leave_formal_params();

    value_manager.leave_func(); // func_id is global identifier
    auto id_koopa = value_manager.new_id(
        koopa::id_type::FuncId,
        new koopa::FuncType(
            param_types,
            ret_type->to_koopa()
        ),
        '@' + id->lit,
        id->nesting_info
    );
    value_manager.enter_func(id->lit);

    auto block_koopa = new koopa_trans::Blocks;
    for (int i = 0; i < param_types.size(); i++) {
        auto pointer_style_param_id = value_manager.new_id(
            koopa::id_type::LocalId,
            new koopa::Pointer(param_types[i]),
            '%' + std::get<1>(*params[i])->lit,
            std::get<1>(*params[i])->nesting_info
        );
        *block_koopa += new koopa::SymbolDef(
            pointer_style_param_id,
            new koopa::MemoryDecl(param_types[i])
        );
        *block_koopa += new koopa::StoreValue(
            param_ids[i],
            pointer_style_param_id
        );
    }
    *block_koopa += *block->to_koopa();
    trim_redundant_stmts_after_end_stmt(block_koopa, ret_type->to_koopa());
    add_pred_succ(block_koopa);

    value_manager.leave_func();

    return new koopa_trans::GlobalStmts(
        new koopa::FuncDef(
            id_koopa,
            param_ids,
            block_koopa->to_raw_blocks()
        )
    );
}

/* push the declaration of lib functions */
static void push_lib_func_decls(std::vector<koopa::GlobalStmt *> &global_stmts_koopa) {
    global_stmts_koopa.push_back(
        new koopa::FuncDecl(
            value_manager.new_id(
                koopa::id_type::FuncId,
                new koopa::FuncType(
                    {}, new koopa::Int
                ),
                "@getint"
            )
        )
    );
    global_stmts_koopa.push_back(
        new koopa::FuncDecl(
            value_manager.new_id(
                koopa::id_type::FuncId,
                new koopa::FuncType(
                    {}, new koopa::Int
                ),
                "@getch"
            )
        )
    );
    global_stmts_koopa.push_back(
        new koopa::FuncDecl(
            value_manager.new_id(
                koopa::id_type::FuncId,
                new koopa::FuncType(
                    { new koopa::Pointer(new koopa::Int) }, 
                    new koopa::Int
                ),
                "@getarray"
            )
        )
    );
    global_stmts_koopa.push_back(
        new koopa::FuncDecl(
            value_manager.new_id(
                koopa::id_type::FuncId,
                new koopa::FuncType(
                    { new koopa::Int }, 
                    new koopa::Void
                ),
                "@putint"
            )
        )
    );
    global_stmts_koopa.push_back(
        new koopa::FuncDecl(
            value_manager.new_id(
                koopa::id_type::FuncId,
                new koopa::FuncType(
                    { new koopa::Int }, 
                    new koopa::Void
                ),
                "@putch"
            )
        )
    );
    global_stmts_koopa.push_back(
        new koopa::FuncDecl(
            value_manager.new_id(
                koopa::id_type::FuncId,
                new koopa::FuncType(
                    { new koopa::Int, new koopa::Pointer(new koopa::Int) }, 
                    new koopa::Void
                ),
                "@putarray"
            )
        )
    );
    global_stmts_koopa.push_back(
        new koopa::FuncDecl(
            value_manager.new_id(
                koopa::id_type::FuncId,
                new koopa::FuncType(
                    {}, new koopa::Void
                ),
                "@starttime"
            )
        )
    );
    global_stmts_koopa.push_back(
        new koopa::FuncDecl(
            value_manager.new_id(
                koopa::id_type::FuncId,
                new koopa::FuncType(
                    {}, new koopa::Void
                ),
                "@endtime"
            )
        )
    );
}

koopa::Program *CompUnit::to_koopa() const {
    auto *global_stmts_koopa = new koopa_trans::GlobalStmts();

    /* the definition of lib functions are offered by .o file */
    push_lib_func_decls(global_stmts_koopa->to_raw_vector());

    for (auto global_stmt: global_stmts) {
        *global_stmts_koopa += *global_stmt->to_koopa();
    }
    
    return new koopa::Program(
        global_stmts_koopa->to_raw_vector()
    );
}

}