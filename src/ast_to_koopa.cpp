#include "../include/ast.h"
#include "../include/koopa.h"
#include "../include/koopa_trans.h"
#include "../include/nesting_info.h"
#include "../include/value_manager.h"
#include "../include/name.h"
#include "../include/loop_tag.h"
#include "../include/aggregate_agent.h"

#include <string>
#include <functional>

namespace ast {

/**
 * generate `new_id`, which is defined as `new_id = lv_koopa op rv_koopa`
 * push `new_id = lv_koopa op rv_koopa` to res
 * @return new_id
 */
template <typename KoopaExpr>
static koopa::Value* generate_binary_expr(
    koopa_trans::Blocks* dest,
    koopa::Value* lv_koopa, 
    koopa::Value* rv_koopa
) {

    auto new_id = value_manager.new_id(
        koopa::id_type::LocalId,
        new koopa::Int,
        new_id_name(),
        new NestingInfo
    );

* dest += new koopa::SymbolDef(
        new_id,
        new KoopaExpr(
            lv_koopa, rv_koopa
        )
    );

    return new_id;
};

static bool is_compile_time_constand(
    koopa_trans::Blocks* lv_stmts, koopa_trans::Blocks* rv_stmts
) {
    return lv_stmts->get_last_val()->is_const() && rv_stmts->get_last_val()->is_const();
}

static koopa_trans::Blocks* build_compile_time_constant(
    koopa_trans::Blocks* lv_stmts, koopa_trans::Blocks* rv_stmts,
    std::function<int(int, int)> func
) {
    return new koopa_trans::Blocks(value_manager.new_const(
        func(lv_stmts->get_last_val()->get_val(), rv_stmts->get_last_val()->get_val())
    ));
}

template <typename KoopaExpr>
static koopa_trans::Blocks* build_ordinary_binary_blocks(
    koopa_trans::Blocks* lv_stmts, koopa_trans::Blocks* rv_stmts,
    std::function<int(int, int)> func
) {
    if (is_compile_time_constand(lv_stmts, rv_stmts)) {
        return build_compile_time_constant(lv_stmts, rv_stmts, func);
    }

    auto res = new koopa_trans::Blocks;
* res +=* lv_stmts;
* res +=* rv_stmts;
    res->set_last_val(
        generate_binary_expr<KoopaExpr>(res, lv_stmts->get_last_val(), rv_stmts->get_last_val())
    );
    return res;
}
enum ShortCircuitBinaryOp { LOGIC_AND, LOGIC_OR };

static koopa_trans::Blocks* build_short_circuit_evaluation(
    ShortCircuitBinaryOp op,
    ast::Expr* lv,
    ast::Expr* rv
) {
    /*
     *  build short circuit evaluation
     *
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
    auto res = new koopa_trans::Blocks;

    auto res_addr = value_manager.new_id(
        koopa::id_type::LocalId, 
        new koopa::Pointer(new koopa::Int), 
        new_id_name()
    );

    auto then_block = new koopa_trans::Blocks;
    auto end_block = new koopa_trans::Blocks;

* res += new koopa::SymbolDef(res_addr, new koopa::MemoryDecl(new koopa::Int));

    auto init_bool_val = [&](
        ast::Expr* val,
        koopa_trans::Blocks* dest
    ) -> koopa::Value * {
        auto stmts = ast::Neq(val, new ast::Number(0)).to_koopa();
* dest +=* stmts;
        return stmts->get_last_val();
    };

    koopa::Value* bool_lv = init_bool_val(lv, res);
* res += new koopa::StoreValue(bool_lv, res_addr);

    if (op == LOGIC_AND) {
* res += new koopa::Branch(
            bool_lv, 
            then_block->get_begin_block_id(), 
            end_block->get_begin_block_id()
        );
    }
    else /* op == LOGIC_OR */ {
* res += new koopa::Branch(
            bool_lv, 
            end_block->get_begin_block_id(), 
            then_block->get_begin_block_id()
        );
    }

    koopa::Value* bool_rv = init_bool_val(rv, then_block);
* then_block += new koopa::StoreValue(bool_rv, res_addr);
* then_block += new koopa::Jump(end_block->get_begin_block_id());

* res += then_block->to_raw_blocks();
* res += end_block->to_raw_blocks();

    auto res_id = value_manager.new_id(koopa::id_type::LocalId, new koopa::Int, new_id_name());
* res += new koopa::SymbolDef(res_id, new koopa::Load(res_addr));
    
    res->set_last_val(res_id);
    return res;
}

koopa_trans::Blocks* LogicOr::to_koopa() const {
    if (rv->has_side_effect()) {
        return build_short_circuit_evaluation(LOGIC_OR, lv, rv);
    }

    auto lv_stmts = Neq(lv, new Number(0)).to_koopa();
    auto rv_stmts = Neq(rv, new Number(0)).to_koopa();

    for (auto stmts: {lv_stmts, rv_stmts}) {
        if (stmts->get_last_val()->is_const() && stmts->get_last_val()->get_val() != 0) {
            return new koopa_trans::Blocks(new koopa::Const(1));
        }
    }

    return build_ordinary_binary_blocks<koopa::Or>(
        lv_stmts, rv_stmts, 
        [](int a, int b) { return a || b; }
    );
}

koopa_trans::Blocks* LogicAnd::to_koopa() const {
    if (rv->has_side_effect()) {
        return build_short_circuit_evaluation(LOGIC_AND, lv, rv);
    }

    auto lv_stmts = Neq(lv, new Number(0)).to_koopa();
    auto rv_stmts = Neq(rv, new Number(0)).to_koopa();

    for (auto stmts: {lv_stmts, rv_stmts}) {
        if (stmts->get_last_val()->is_const() && stmts->get_last_val()->get_val() == 0) {
            return new koopa_trans::Blocks(new koopa::Const(0));
        }
    }

    return build_ordinary_binary_blocks<koopa::And>(
        lv_stmts, rv_stmts, 
        [](int a, int b) { return a && b; }
    );
}

koopa_trans::Blocks* Assign::to_koopa() const {
    if (typeid(* lv) != typeid(ast::Id)) {
        throw "try to declare a non-id";
    }


    auto res = new koopa_trans::Blocks;
    auto rv_stmts = rv->to_koopa();
    auto id = static_cast<ast::Id*>(lv);

* res +=* rv_stmts;

    auto id_koopa = value_manager.get_id('@' + id->lit, id->nesting_info);

    if (id_koopa == nullptr) {
        throw "undeclared identifier `" + id->lit + '`';
    }
    if (id_koopa->is_const()) {
        throw "assigning to a const identifier `" + id->lit + '`';
    }

* res += new koopa::StoreValue(
        res->get_last_val(),
        value_manager.get_id('@' + id->lit, id->nesting_info)
    );

    res->set_last_val( id_koopa );

    return res;
}

koopa_trans::Blocks* Comma::to_koopa() const {
    auto lv_stmts = lv->to_koopa();
    auto rv_stmts = rv->to_koopa();

    if (is_compile_time_constand(lv_stmts, rv_stmts)) {
        return build_compile_time_constant(
            lv_stmts, rv_stmts, 
            [](int a, int b) { return b; }
        );
    }

    auto res = new koopa_trans::Blocks;
    
    if (lv->has_side_effect())* res +=* lv_stmts;
    if (rv_stmts->get_last_val()->is_const()) {
        res->set_last_val( rv_stmts->get_last_val() );
    }
    else {
* res +=* rv_stmts;
        res->set_last_val( rv_stmts->get_last_val() );
    }
    return res;
}

koopa_trans::Blocks* Eq::to_koopa() const {
    return build_ordinary_binary_blocks<koopa::Eq>(
        lv->to_koopa(), rv->to_koopa(), 
        [](int a, int b) { return a == b; }
    );
}

koopa_trans::Blocks* Neq::to_koopa() const {
    return build_ordinary_binary_blocks<koopa::Ne>(
		lv->to_koopa(), rv->to_koopa(),
		[](int a, int b) { return a != b; }
	);
}

koopa_trans::Blocks* Lt::to_koopa() const {
    return build_ordinary_binary_blocks<koopa::Lt>(
		lv->to_koopa(), rv->to_koopa(),
		[](int a, int b) { return a < b; }
	);
}

koopa_trans::Blocks* Gt::to_koopa() const {
    return build_ordinary_binary_blocks<koopa::Gt>(
		lv->to_koopa(), rv->to_koopa(),
		[](int a, int b) { return a > b; }
	);
}

koopa_trans::Blocks* Leq::to_koopa() const {
    return build_ordinary_binary_blocks<koopa::Le>(
		lv->to_koopa(), rv->to_koopa(),
		[](int a, int b) { return a <= b; }
	);
}

koopa_trans::Blocks* Geq::to_koopa() const {
    return build_ordinary_binary_blocks<koopa::Ge>(
		lv->to_koopa(), rv->to_koopa(),
		[](int a, int b) { return a >= b; }
	);
}

koopa_trans::Blocks* Add::to_koopa() const {
    auto lv_stmts = lv->to_koopa();
    auto rv_stmts = rv->to_koopa();

    for (auto pair: {
        std::make_tuple(lv_stmts, rv_stmts),
        std::make_tuple(rv_stmts, lv_stmts)
    }) {
        if (
            std::get<0>(pair)->get_last_val()->is_const() 
            && std::get<0>(pair)->get_last_val()->get_val() == 0) {
            return std::get<1>(pair);
        }
    }

    return build_ordinary_binary_blocks<koopa::Add>(
		lv_stmts, rv_stmts,
		[](int a, int b) { return a + b; }
	);
}

koopa_trans::Blocks* Sub::to_koopa() const {
    auto lv_stmts = lv->to_koopa();
    auto rv_stmts = rv->to_koopa();
    
    if (rv_stmts->get_last_val()->is_const() && rv_stmts->get_last_val()->get_val() == 0) {
        return lv_stmts;
    }
    
    return build_ordinary_binary_blocks<koopa::Sub>(
		lv_stmts, rv_stmts,
		[](int a, int b) { return a - b; }
	);
}

koopa_trans::Blocks* Mul::to_koopa() const {
    auto lv_stmts = lv->to_koopa();
    auto rv_stmts = rv->to_koopa();
    
    for (auto pair: {
        std::make_tuple(lv_stmts, rv_stmts),
        std::make_tuple(rv_stmts, lv_stmts)
    }) {
        if (
            std::get<0>(pair)->get_last_val()->is_const() 
            && std::get<0>(pair)->get_last_val()->get_val() == 1) {
            return std::get<1>(pair);
        }
    }
    
    return build_ordinary_binary_blocks<koopa::Mul>(
		lv_stmts, rv_stmts,
		[](int a, int b) { return a * b; }
	);
}

koopa_trans::Blocks* Div::to_koopa() const {
    auto lv_stmts = lv->to_koopa();
    auto rv_stmts = rv->to_koopa();
    
    if (rv_stmts->get_last_val()->is_const() && rv_stmts->get_last_val()->get_val() == 1) {
        return lv_stmts;
    }

    return build_ordinary_binary_blocks<koopa::Div>(
		lv->to_koopa(), rv->to_koopa(),
		[](int a, int b) { return a / b; }
	);
}

koopa_trans::Blocks* Mod::to_koopa() const {
    return build_ordinary_binary_blocks<koopa::Mod>(
		lv->to_koopa(), rv->to_koopa(),
		[](int a, int b) { return a % b; }
	);
}

koopa_trans::Blocks* Pos::to_koopa() const {
    return lv->to_koopa();
}

template <typename KoopaExpr>
static koopa_trans::Blocks* build_ordinary_unary_blocks(
    koopa_trans::Blocks* lv_stmts, std::function<int(int)> func
) {
    if (lv_stmts->get_last_val()->is_const()) {
        return new koopa_trans::Blocks(
            value_manager.new_const(func(lv_stmts->get_last_val()->get_val()))
        );
    }

    auto new_id = value_manager.new_id(
        koopa::id_type::LocalId,
        new koopa::Int,
        new_id_name(),
        new NestingInfo
    );

* lv_stmts += new koopa::SymbolDef(
        new_id,
        new KoopaExpr(
            value_manager.new_const(0),
            lv_stmts->get_last_val()
        )
    );

    lv_stmts->set_last_val(new_id);

    return lv_stmts;
}

koopa_trans::Blocks* Neg::to_koopa() const {
    return build_ordinary_unary_blocks<koopa::Sub>(
        lv->to_koopa(), [](int a){ return -a; }
    );
}

koopa_trans::Blocks* Not::to_koopa() const {
    return build_ordinary_unary_blocks<koopa::Eq>(
        lv->to_koopa(), [](int a){ return !a; }
    );
}

koopa_trans::Blocks* Number::to_koopa() const {
    return new koopa_trans::Blocks(value_manager.new_const(val));
}

koopa::Initializer* ConstInitializer::initializer_to_koopa(
    std::vector<int> dimensions
) const {
    auto val_koopa = val->to_koopa()->get_last_val();
    if (!val_koopa->is_const()) {
        throw "using expression `" + val->debug() 
            + "` in an initialization list "
            "that is not compile-time evaluable";
    }
    return new koopa::ConstInitializer(val_koopa->get_val());
}

void ConstInitializer::initializer_to_koopa_agent(
    AggregateAgent& agent
) const {
    auto val_koopa = val->to_koopa()->get_last_val();
    if (!val_koopa->is_const()) {
        throw "using expression `" + val->debug() 
            + "` in an initialization list "
            "that is not compile-time evaluable";
    }
    agent.fill(val_koopa->get_val());
}

koopa::Initializer* Aggregate::initializer_to_koopa(
    std::vector<int> dimensions
) const {
    auto agent = AggregateAgent(dimensions); 
    initializer_to_koopa_agent(agent);
    return agent.to_aggregate();
}

void Aggregate::initializer_to_koopa_agent(
    AggregateAgent& agent
) const {
    agent.enter_aggregate();

    if (initializers.size() == 0) {
        agent.fill(0);
    }
    for (auto initializer: initializers) {
        initializer->initializer_to_koopa_agent(agent);
    }

    agent.leave_aggregate();
}

koopa_trans::Blocks* ConstInitializer::expr_to_koopa() const {
    return val->to_koopa();
}

koopa_trans::Blocks* Aggregate::expr_to_koopa() const {
    assert(this->get_dim() == 0); // always fail
    return nullptr;
}

koopa_trans::Blocks* Id::to_koopa() const {
    auto res = new koopa_trans::Blocks;

    auto id_koopa = value_manager.get_id('@' + lit, nesting_info);

    if (id_koopa == nullptr) {
        throw "undeclared identifier `" + lit + '`';
    }

    if (id_koopa->is_const()) {
        return new koopa_trans::Blocks(value_manager.new_const(id_koopa->get_val()));
    }
    else {

        auto new_id = value_manager.new_id(
            koopa::id_type::LocalId,
            //!? re-free bug
            static_cast<koopa::Pointer*>(id_koopa->type)->pointed_type, 
            new_id_name(),
            new NestingInfo
        );

* res += new koopa::SymbolDef(
            new_id,
            new koopa::Load(id_koopa)
        );

        res->set_last_val( new_id );

        return res;

    }
}

bool is_consistent(koopa::FuncType* func_type, std::vector<Expr*> actual_params) {
    if (func_type->arg_types.size() != actual_params.size()) return false;
    
    // TODO  check param type

    return true;
}

koopa_trans::Blocks* FuncCall::to_koopa() const {
    auto res = new koopa_trans::Blocks;

    auto func_id_koopa = value_manager.get_func_id('@' + func_id->lit, func_id->nesting_info);

    if (func_id_koopa == nullptr) {
        throw "call of function `" + func_id->lit + "` undeclared";
    }

    if (!is_consistent(
            dynamic_cast<koopa::FuncType*>(func_id_koopa->type), 
            actual_params
        )) {
        throw "calling function `" + func_id->lit + "` with mismatched actual arguments";
    }

    auto actual_params_koopa = std::vector<koopa::Value*>();
    actual_params_koopa.reserve(actual_params.size());
    for (auto actual_param: actual_params) {
        auto actual_param_koopa = actual_param->to_koopa();
* res +=* actual_param_koopa;
        actual_params_koopa.push_back(actual_param_koopa->get_last_val());
    }

    auto ret_type_koopa = dynamic_cast<koopa::FuncType*>(func_id_koopa->type)->ret_type;
    if (ret_type_koopa->get_type_id() == koopa::type::Void) {
* res += new koopa::FuncCall(func_id_koopa, actual_params_koopa);
        res->throw_last_val();
    }
    else {
        auto res_id = value_manager.new_id(
            koopa::id_type::LocalId,
            ret_type_koopa,
            new_id_name()
        );
* res += new koopa::SymbolDef(
            res_id,
            new koopa::FuncCall(func_id_koopa, actual_params_koopa)
        );
        res->set_last_val( res_id );
    }

    return res;
}

koopa_trans::GlobalStmts* VolatileGlobalVarDef::to_koopa() const {
    if (value_manager.is_id_declared('@' + id->lit, id->nesting_info)) {
        throw '`' + id->lit + "` redefined";
    }

    auto stmts = new koopa_trans::GlobalStmts;
    auto type_koopa = type->to_koopa();

    if (type_koopa->get_dim().size() == 0) {
        koopa::Initializer* init_koopa;
        
        if (has_init) {
            if (init->get_dim() > 0) {
                throw "initialize non-array object `" + id->debug() 
                    + "` with aggregate`" + init->debug() + '`';
            }
            
            auto rval_koopa = init->expr_to_koopa();

            if (!rval_koopa->get_last_val()->is_const()) {
                throw "initiating global variable `" 
                    + id->lit + "` with a non-const value";
            }

            init_koopa = new koopa::ConstInitializer(rval_koopa->get_last_val()->get_val());
        }
        else /* !has_init */ { 
            init_koopa = new koopa::Zeroinit;
        }

* stmts += new koopa::GlobalSymbolDef(
            value_manager.new_id(
                koopa::id_type::GlobalId,
                new koopa::Pointer(
                    type_koopa
                ),
                '@' + id->lit,
                id->nesting_info
            ),
            new koopa::GlobalMemoryDecl(
                type_koopa,
                init_koopa
            )
        );
    }
    else /* type->get_dim() > 0 */ { 
        koopa::Initializer* init_koopa;
        
        if (has_init) {
            if (init->get_dim() == 0) {
                throw "initialize array object `" + id->debug() 
                        + "` with value`" + init->debug() + '`';
            }

            init_koopa = init->initializer_to_koopa(type_koopa->get_dim());
        }
        else /* !has_init */ { 
            init_koopa = new koopa::Zeroinit;
        }

* stmts += new koopa::GlobalSymbolDef(
            value_manager.new_id(
                koopa::id_type::GlobalId,
                new koopa::Pointer(
                    type_koopa
                ),
                '@' + id->lit,
                id->nesting_info
            ),
            new koopa::GlobalMemoryDecl(
                type_koopa,
                init_koopa
            )
        );
    }

    return stmts;
}

koopa_trans::GlobalStmts* ConstGlobalVarDef::to_koopa() const {
    if (value_manager.is_id_declared('@' + id->lit, id->nesting_info)) {
        throw '`' + id->lit + "` redefined";
    }

    if (!has_init) {
        throw "no initiator for const variable `" + id->lit + '`';
    }
    
    auto type_koopa = type->to_koopa();

    if (type_koopa->get_dim().size() == 0) {
        if (init->get_dim() > 0) {
            throw "initialize non-array object `" + id->debug() 
                + "` with aggregate`" + init->debug() + '`';
        }

        auto stmts = new koopa_trans::GlobalStmts;
        auto rval_koopa = init->expr_to_koopa();

        if (!rval_koopa->get_last_val()->is_const()) {
            throw "initiating const variable `" + id->lit + "` with a non-const value";
        }

        value_manager.new_id(
            koopa::id_type::GlobalId,
            type_koopa,
            '@' + id->lit,
            rval_koopa->get_last_val()->get_val(),
            id->nesting_info
        );

        return stmts;
    }
    else /* type->get_dim() > 0 */ { 
        return VolatileGlobalVarDef(type, id, init).to_koopa();
    }
}

koopa_trans::GlobalStmts* GlobalVarDecl::to_koopa() const {
    auto stmts = new koopa_trans::GlobalStmts;

    for (auto var_def: var_defs) {
* stmts +=* var_def->to_koopa();
    }

    return stmts;
}

koopa_trans::Blocks* VolatileVarDef::to_koopa() const {
    if (value_manager.is_id_declared('@' + id->lit, id->nesting_info)) {
        throw '`' + id->lit + "` redefined";
    }

    auto stmts = new koopa_trans::Blocks;
    auto type_koopa = type->to_koopa();

    if (type_koopa->get_dim().size() == 0) {
* stmts += new koopa::SymbolDef(
            value_manager.new_id(
                koopa::id_type::LocalId,
                new koopa::Pointer(
                    type_koopa
                ),
                '@' + id->lit,
                id->nesting_info
            ),
            new koopa::MemoryDecl(type_koopa)
        );

        if (has_init) {
            if (init->get_dim() > 0) {
                throw "initialize non-array object `" + id->debug() 
                    + "` with aggregate`" + init->debug() + '`';
            }

            auto rval_koopa = init->expr_to_koopa();
* stmts +=* rval_koopa;
* stmts += new koopa::StoreValue(
                rval_koopa->get_last_val(),
                value_manager.get_id('@' + id->lit, id->nesting_info)
            );
        }
    }
    else /* type->get_dim() > 0 */ { 
* stmts += new koopa::SymbolDef(
            value_manager.new_id(
                koopa::id_type::LocalId,
                new koopa::Pointer(
                    type_koopa
                ),
                '@' + id->lit,
                id->nesting_info
            ),
            new koopa::MemoryDecl(type_koopa)
        );

        if (has_init) {
            if (init->get_dim() == 0) {
                throw "initialize array object `" + id->debug() 
                    + "` with primitive value`" + init->debug() + '`';
            }

* stmts += new koopa::StoreInitializer(
                init->initializer_to_koopa(type_koopa->get_dim()),
                value_manager.get_id('@' + id->lit, id->nesting_info)
            );
        }
    }

    return stmts;
}

koopa_trans::Blocks* ConstVarDef::to_koopa() const {
    if (value_manager.is_id_declared('@' + id->lit, id->nesting_info)) {
        throw '`' + id->lit + "` redefined";
    }

    if (!has_init) {
        throw "no initiator for const variable `" + id->lit + '`';
    }

    auto stmts = new koopa_trans::Blocks;
    auto type_koopa = type->to_koopa();

    if (type_koopa->get_dim().size() == 0) {
        if (init->get_dim() > 0) {
            throw "initialize non-array object `" + id->debug() 
                + "` with aggregate`" + init->debug() + '`';
        }

        auto rval_koopa = init->expr_to_koopa();

        if (!rval_koopa->get_last_val()->is_const()) {
            throw "initiating const variable `" 
                + id->lit + "` with a non-const value";
        }

        value_manager.new_id(
            koopa::id_type::LocalId,
            type_koopa,
            '@' + id->lit,
            rval_koopa->get_last_val()->get_val(),
            id->nesting_info
        );
    }
    else /* type->get_dim() > 0 */ { 
        return VolatileVarDef(type, id, init).to_koopa();
    }

    return stmts;
}

koopa_trans::Blocks* VarDecl::to_koopa() const {
    auto stmts = new koopa_trans::Blocks;

    for (auto var_def: var_defs) {
* stmts +=* var_def->to_koopa();
    }

    return stmts;
}

koopa_trans::Blocks* Return::to_koopa() const {
    if (return_type == return_type::NotHasRetVal) {
        return new koopa_trans::Blocks({new koopa::Return});
    }

    auto res = ret_val->to_koopa();
* res += new koopa::Return( res->get_last_val() );
    return res;
}

koopa_trans::Blocks* If::to_koopa() const {
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
* res +=* cond->to_koopa();

        auto cond_koopa = res->get_last_val();

        auto then_blocks = then_stmt->to_koopa()->to_raw_blocks();
        auto else_blocks = else_stmt->to_koopa()->to_raw_blocks();
        auto end_block = new koopa::Block(
            value_manager.new_id(
                koopa::id_type::BlockLabel,
                new koopa::Label,   
                new_block_name(), 
                new NestingInfo
            ),
            {}
        );

* res += new koopa::Branch(
            cond_koopa,
            then_blocks.front()->id,
            else_blocks.front()->id
        );

* then_blocks.back() += new koopa::Jump(end_block->id);
* else_blocks.back() += new koopa::Jump(end_block->id);

* res += then_blocks;
* res += else_blocks;
* res += end_block;
    }
    else {  /* !has_else_stmt */
* res +=* cond->to_koopa();

        auto cond_koopa = res->get_last_val();

        auto then_blocks = then_stmt->to_koopa()->to_raw_blocks();
        auto end_block = new koopa::Block(
            value_manager.new_id(
                koopa::id_type::BlockLabel,
                new koopa::Label,   
                new_block_name(), 
                new NestingInfo
            ),
            {}
        );

* res += new koopa::Branch(
            cond_koopa,
            then_blocks.front()->id,
            end_block->id
        );

* then_blocks.back() += new koopa::Jump(end_block->id);

* res += then_blocks;
* res += end_block;
    }

    return res;
}

koopa_trans::Blocks* While::to_koopa() const {
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

* res += new koopa::Jump(while_entry->get_begin_block_id());

    auto cond_koopa = cond->to_koopa();
* while_entry +=* cond_koopa;
* while_entry += new koopa::Branch(
        cond_koopa->get_last_val(), 
        while_body->get_begin_block_id(), 
        while_end->get_begin_block_id()
    );

    auto body_koopa = body->to_koopa();
* while_body +=* body_koopa;
* while_body += new koopa::Jump(while_entry->get_begin_block_id());

* res += while_entry->to_raw_blocks();
* res += while_body->to_raw_blocks();
* res += while_end->to_raw_blocks();

    loop_tag_manager.pop();

    return res;
}

koopa_trans::Blocks* For::to_koopa() const {
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

* res +=* init_stmt->to_koopa();
* res += new koopa::Jump(for_entry->get_begin_block_id());

    auto cond_koopa = cond->to_koopa();
* for_entry +=* cond_koopa;
* for_entry += new koopa::Branch(
        cond_koopa->get_last_val(), 
        for_body->get_begin_block_id(), 
        for_end->get_begin_block_id()
    );

    auto body_koopa = body->to_koopa();
* for_body +=* body_koopa;
* for_body += new koopa::Jump(for_iter->get_begin_block_id());

    auto iter_koopa = iter_stmt->to_koopa();
* for_iter +=* iter_koopa;
* for_iter += new koopa::Jump(for_entry->get_begin_block_id());

* res += for_entry->to_raw_blocks();
* res += for_body->to_raw_blocks();
* res += for_iter->to_raw_blocks();
* res += for_end->to_raw_blocks();

    loop_tag_manager.pop();

     return res;
}

koopa_trans::Blocks* Continue::to_koopa() const {
    if (loop_tag_manager.empty()) {
        throw "not using `continue` in loop";
    }
    return new koopa_trans::Blocks(
        { new koopa::Jump(loop_tag_manager.top().continue_target) }
    );
}

koopa_trans::Blocks* Break::to_koopa() const {
    if (loop_tag_manager.empty()) {
        throw "not using `break` in loop";
    }
    return new koopa_trans::Blocks(
        { new koopa::Jump(loop_tag_manager.top().break_target) }
    );
}

static void trim_redundant_stmts_after_end_stmt(
    std::vector<koopa::Stmt*>& stmts, 
    koopa::Type* ret_type
) {
    bool exist_end_stmt = false;

    for (auto it = stmts.begin(); it != stmts.end(); it = std::next(it)) {
        if ((* it)->is_end_stmt()) {
            exist_end_stmt = true;
            stmts.erase(std::next(it), stmts.end());
            break;
        }
    }

    if (!exist_end_stmt) {
        koopa::Return* ret_stmt = nullptr;
        if (ret_type->get_type_id() == koopa::type::Int) {
            ret_stmt = new koopa::Return(value_manager.new_const(0));
        }
        else if (ret_type->get_type_id() == koopa::type::Void) {
            ret_stmt = new koopa::Return;
        }
        else {
            // TODO
        }
        stmts.push_back(ret_stmt);
    }
}

static void trim_redundant_stmts_after_end_stmt(
    koopa_trans::Blocks* blocks, 
    koopa::Type* ret_type
) {
    trim_redundant_stmts_after_end_stmt(blocks->active_stmts, ret_type);
    for (auto block: blocks->blocks) {
        trim_redundant_stmts_after_end_stmt(block->stmts, ret_type);
    }
}

static void add_pred_succ(koopa_trans::Blocks* blocks) {
    //TODO
}

koopa_trans::Blocks* Block::to_koopa() const {
    auto res = new koopa_trans::Blocks;

    for (auto stmt: stmts) {
* res +=* stmt->to_koopa();
    }

    return res;
}

koopa::Type* Int::to_koopa() const {
    return new koopa::Int;
}

koopa::Type* Void::to_koopa() const {
    return new koopa::Void;
}

koopa::Type* Pointer::to_koopa() const {
    return new koopa::Pointer(pointed_type->to_koopa());
}

koopa::Type* Array::to_koopa() const {
    auto length_koopa = length->to_koopa();
    if (!length_koopa->get_last_val()->is_const()) {
        throw "declaring array with a variant length `" + length->debug() + "`";
    }
    if (length_koopa->get_last_val()->get_val() <= 0) {
        throw "declaring array with a variant non-positive length `" + length->debug() + "`";
    }
    return new koopa::Array(
        element_type->to_koopa(), 
        length_koopa->get_last_val()->get_val()
    );
}


koopa_trans::GlobalStmts* FuncDef::to_koopa() const {
    if (value_manager.get_id('@' + id->lit, id->nesting_info) != nullptr) {
        throw "`" + id->lit + "` redefined";
    }

    value_manager.enter_func(id->lit);

    auto param_types = std::vector<koopa::Type*>();
    auto param_ids = std::vector<koopa::Id*>();

    value_manager.enter_formal_params();
    param_types.reserve(params.size());
    param_ids.reserve(params.size());
    for (auto param: params) {
        auto param_type_koopa = std::get<0>(* param)->to_koopa();
        auto param_id_koopa = value_manager.new_id(
            koopa::id_type::LocalId,
            param_type_koopa, 
            '@' + std::get<1>(* param)->lit,
            std::get<1>(* param)->nesting_info
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
            '%' + std::get<1>(* params[i])->lit,
            std::get<1>(* params[i])->nesting_info
        );
* block_koopa += new koopa::SymbolDef(
            pointer_style_param_id,
            new koopa::MemoryDecl(param_types[i])
        );
* block_koopa += new koopa::StoreValue(
            param_ids[i],
            pointer_style_param_id
        );
    }
* block_koopa +=* block->to_koopa();
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
static void push_lib_func_decls(std::vector<koopa::GlobalStmt*>& global_stmts_koopa) {
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

koopa::Program* CompUnit::to_koopa() const {
    auto* global_stmts_koopa = new koopa_trans::GlobalStmts;

    /* the definition of lib functions are offered by .o file */
    push_lib_func_decls(global_stmts_koopa->to_raw_vector());

    for (auto global_stmt: global_stmts) {
* global_stmts_koopa +=* global_stmt->to_koopa();
    }
    
    return new koopa::Program(
        global_stmts_koopa->to_raw_vector()
    );
}

}