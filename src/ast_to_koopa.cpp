#include "../include/ast.h"
#include "../include/koopa.h"

#include <string>

static int block_count = 0;

namespace koopa_trans {
    /**
     *
     *  serve as koopa translation intermediate variable
     *
     *  structure:
     *
     *          stmt        ----+
     *          stmt            |====> active_stmts
     *          ...             |       * do not belong to any block
     *          stmt        ----+
     *                                  * will be merged into the active block when merging two `Blocks` or converted
     *      block           ----+         into a block by calling `to_raw_blocks`
     *          stmt            |
     *          stmt            |====> blocks
     *          ...             |       * may be empty
     *          stmt            |
     *                          |       * the last block is active if one exists; otherwise, `active_stmts` would be treated
     *      block               |         as an active block
     *          stmt            |
     *          stmt            |       * when merging two `Blocks`, the `active_stmts` of the latter one will be merged into the
     *          ...             |         active block of the first, and then `blocks` will be appended to the first
     *          stmt            |
     *      ...             ----+
     *
     *      when `blocks` is empty, `last_val` saves the pointer of last value active_stmts defines, for instance in 
     *          %1 = add %0, 1
     *          %2 = mul %1, 2
     *      , `last_val` = %2, callee may capture and use it in following compiling. the `last_val` of a `Blocks` with
     *      not empty `blocks` is undefined. when merging two `Blocks`, the last_val of new `Blocks` will the latter one's.
     *
     *  examples:
     *
     *      stmts][block][block] + stmts][block]  =>  stmts][block][block+stmts][block]
     *                      ^               ^                                      ^
     *                    active          active                                 active
     *
     *      stmts1] + stmts2][block]  =>  stmts1+stmts2][block]
     *        ^                 ^                          ^
     *      active            active                     active
     *
     *      stmts][block][block].to_raw_blocks  =>  [stmts][block][block]
     *
     */
    class Blocks : public koopa::Base {
    public:
        std::vector<koopa::Stmt *> active_stmts = {};
        koopa::Value *last_val = nullptr;

        std::vector<koopa::Block *> blocks = {};

        Blocks(std::vector<koopa::Stmt *> stmts = {}, koopa::Value *last_val = nullptr) 
            : active_stmts(stmts), last_val(last_val) {}

        std::vector<koopa::Block *> to_raw_blocks();

        friend void operator+=(Blocks &self, Blocks &other);
        friend void operator+=(Blocks &self, std::vector<koopa::Stmt *> &stmts);
        friend void operator+=(Blocks &self, koopa::Stmt &stmt);

        std::string to_string() const { return ""; }
        void to_riscv(std::string &str, riscv_trans::Info &info) const {}
    };

    std::vector<koopa::Block *> Blocks::to_raw_blocks() {
        auto res = std::vector<koopa::Block *>{};
        res.reserve(1 + blocks.size());

        res.push_back( 
            new koopa::Block(
                new koopa::Id(new koopa::Label, new std::string('%' + std::to_string(block_count++))),
                active_stmts
            ) 
        );

        res.insert(res.end(), blocks.begin(), blocks.end());

        return res;
        //! memory leak
    }

    void operator+=(Blocks &self, Blocks &other) {

        self.last_val = other.last_val;

        if (self.blocks.empty()) {
            self.active_stmts.reserve(self.active_stmts.size() + other.active_stmts.size());
            self.active_stmts.insert(self.active_stmts.end(), other.active_stmts.begin(), other.active_stmts.end());

            self.blocks = other.blocks;
        }
        else {
            *self.blocks.back() += other.active_stmts;

            self.blocks.reserve(self.blocks.size() + other.blocks.size());
            self.blocks.insert(self.blocks.end(), other.blocks.begin(), other.blocks.end());
        }
    }

    void operator+=(Blocks &self, std::vector<koopa::Stmt *> &stmts) {
        if (self.blocks.empty()) {
            self.active_stmts.reserve(self.active_stmts.size() + stmts.size());
            self.active_stmts.insert(self.active_stmts.end(), stmts.begin(), stmts.end());
        }
        else {
            *self.blocks.back() += stmts;
        }
    }

    void operator+=(Blocks &self, koopa::Stmt *stmt) {
        if (self.blocks.empty()) {
            self.active_stmts.push_back(stmt);
        }
        else {
            *self.blocks.back() += stmt;
        }
    }

}

static int tmp_var_count = 0;

koopa::Base *ast::BinaryExpr::to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const {
    auto res = new koopa_trans::Blocks;

    auto lv_stmts = static_cast<koopa_trans::Blocks *>(lv->to_koopa(value_saver, nesting_info));
    auto rv_stmts = static_cast<koopa_trans::Blocks *>(rv->to_koopa(value_saver, nesting_info));

    *res += *lv_stmts;
    *res += *rv_stmts;

    auto generator = [&] (koopa::op::Op op, koopa::Value *elv, koopa::Value *erv) {
        if (elv->is_const && erv->is_const) {
            res->active_stmts.clear();

            res->last_val = value_saver.new_const(
                koopa::op::op_func[op](elv->val, erv->val)
            );

            return;
        }

        res->last_val = value_saver.new_id(
            new koopa::Int,
            new std::string('%' + std::to_string(tmp_var_count++)),
            new ast::NestingInfo(false)
        );

        *res += new koopa::SymbolDef(
            static_cast<koopa::Id *>(res->last_val),
            new koopa::Expr(
                op, elv, erv
            )
        );
    };

    switch (op) {
        case op::LOGIC_AND: {
            generator(koopa::op::NE, lv_stmts->last_val, value_saver.new_const(0));
            auto tmpa = res->last_val;
            generator(koopa::op::NE, rv_stmts->last_val, value_saver.new_const(0));
            auto tmpb = res->last_val;
            generator(koopa::op::AND, tmpa, tmpb);
            break;
        }
        case op::LOGIC_OR: {
            generator(koopa::op::NE, lv_stmts->last_val, value_saver.new_const(0));
            auto tmpa = res->last_val;
            generator(koopa::op::NE, rv_stmts->last_val, value_saver.new_const(0));
            auto tmpb = res->last_val;
            generator(koopa::op::OR, tmpa, tmpb);
            break;
        }
        case op::EQ: generator(koopa::op::EQ, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::NEQ: generator(koopa::op::NE, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::LT: generator(koopa::op::LT, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::GT: generator(koopa::op::GT, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::LEQ: generator(koopa::op::LE, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::GEQ: generator(koopa::op::GE, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::ADD: generator(koopa::op::ADD, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::SUB: generator(koopa::op::SUB, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::MUL: generator(koopa::op::MUL, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::DIV: generator(koopa::op::DIV, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::MOD: generator(koopa::op::MOD, lv_stmts->last_val, rv_stmts->last_val); break;
    }

    delete lv_stmts;
    delete rv_stmts;

    return res;
}

koopa::Base *ast::UnaryExpr::to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const {
    auto lv_stmts = static_cast<koopa_trans::Blocks *>(lv->to_koopa(value_saver, nesting_info));

    auto generator = [&](koopa::op::Op op) {
        if (lv_stmts->last_val->is_const) {
            lv_stmts->active_stmts.clear();

            lv_stmts->last_val = value_saver.new_const(koopa::op::op_func[op](0, lv_stmts->last_val->val));

            return;
        }

        if (op == koopa::op::ADD) return;

        koopa::NotEndStmt *new_stmt;
        koopa::Id *new_id;

        new_id = value_saver.new_id(
            new koopa::Int,
            new std::string('%' + std::to_string(tmp_var_count++)),
            new ast::NestingInfo(false)
        );

        new_stmt = new koopa::SymbolDef(
            new_id,
            new koopa::Expr(
                op,
                value_saver.new_const(0),
                lv_stmts->last_val
            )
        );

        *lv_stmts += new_stmt;
        lv_stmts->last_val = new_id;
    };

    switch (op) {
        case op::POS:

            generator(koopa::op::ADD);

            break;

        case op::NOT:

            generator(koopa::op::EQ);

            break;

        case op::NEG:

            generator(koopa::op::SUB);
    }

    return lv_stmts;
}

koopa::Base *ast::Number::to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const {
    auto res = new koopa_trans::Blocks;
    res->last_val = value_saver.new_const(val);
    return res;
}

koopa::Base *ast::ExprStmt::to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const {
    return expr->to_koopa(value_saver, nesting_info);
}

koopa::Base *ast::Id::to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const {
    auto res = new koopa_trans::Blocks;

    auto id_koopa = value_saver.get_id('@' + *lit, nesting_info);

    if (id_koopa == nullptr) {
        throw "undeclared identifier `" + *lit + '`';
    }

    if (id_koopa->is_const) {

        res->last_val = value_saver.new_const(id_koopa->val);
        return res;

    }
    else {

        auto new_id = value_saver.new_id(
            static_cast<koopa::Pointer *>(id_koopa->type)->pointed_type, //!? re-free bug
            new std::string('%' + std::to_string(tmp_var_count++)),
            new ast::NestingInfo(false)
        );

        *res += new koopa::SymbolDef(
            new_id,
            new koopa::Load(id_koopa)
        );

        res->last_val = new_id;

        return res;

    }
}

koopa::Base *ast::VarDecl::to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const {
    auto stmts = new koopa_trans::Blocks();

    if (is_const) {

        for (auto var_def : var_defs) {
            if (value_saver.is_id_declared('@' + *var_def->id->lit, nesting_info)) {
                throw '`' + *var_def->id->lit + "` redefined";
            }

            if (!var_def->has_init) {
                throw "no initiator for const variable `" + *var_def->id->lit + '`';
            }

            auto rval_koopa = static_cast<koopa_trans::Blocks *>(var_def->init->to_koopa(value_saver, nesting_info));

            if (!rval_koopa->last_val->is_const) {
                throw "initiating const variable `" + *var_def->id->lit + "` with a non-const value";
            }

            value_saver.new_id(
                static_cast<koopa::Type *>(type->to_koopa(value_saver, nesting_info)),
                new std::string('@' + *var_def->id->lit),
                nesting_info,
                true,
                rval_koopa->last_val->val
            );

            //! BUG refree memory
            // for (auto stmt : stmts->stmts) if (stmt != nullptr) delete stmt;
            // delete stmts;
        }

    }

    else {

        for (auto var_def : var_defs) {
            if (value_saver.is_id_declared('@' + *var_def->id->lit, nesting_info)) {
                throw '`' + *var_def->id->lit + "` redefined";
            }

            *stmts += new koopa::SymbolDef(
                value_saver.new_id(
                    new koopa::Pointer(
                        static_cast<koopa::Type *>(type->to_koopa(value_saver, nesting_info))
                    ),
                    new std::string('@' + *var_def->id->lit),
                    nesting_info
                ),
                new koopa::MemoryDecl(static_cast<koopa::Type *>(type->to_koopa(value_saver, nesting_info)))
            );

            if (var_def->has_init) {

                auto rval_koopa = static_cast<koopa_trans::Blocks *>(var_def->init->to_koopa(value_saver, nesting_info));

                *stmts += *rval_koopa;

                *stmts += new koopa::StoreValue(
                    rval_koopa->last_val,
                    value_saver.get_id('@' + *var_def->id->lit, nesting_info)
                );
        }
        }

    }

    return stmts;
}

koopa::Base *ast::Assign::to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const {
    auto res = static_cast<koopa_trans::Blocks *>(rval->to_koopa(value_saver, nesting_info));

    auto id_koopa = value_saver.get_id('@' + *id->lit, nesting_info);

    if (id_koopa == nullptr) {
        throw "undeclared identifier `" + *id->lit + '`';
    }
    if (id_koopa->is_const) {
        throw "assigning to a const identifier `" + *id->lit + '`';
    }

    *res += new koopa::StoreValue(
        res->last_val,
        value_saver.get_id('@' + *id->lit, nesting_info)
    );

    return res;
}

koopa::Base *ast::Return::to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const {
    if (!has_return_val) {
        auto res = new koopa_trans::Blocks();
        *res += new koopa::Return();
        return res;
    }

    auto res = static_cast<koopa_trans::Blocks *>(ret_val->to_koopa(value_saver, nesting_info));

    *res += new koopa::Return( res->last_val );

    return res;
}

koopa::Base *ast::Block::to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const {
    auto res = new koopa_trans::Blocks;

    for (auto stmt : stmts) {
        *res += *static_cast<koopa_trans::Blocks *>(stmt->to_koopa(value_saver, this->nesting_info));
    }

    return res;
}

koopa::Base *ast::Int::to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const {
    return new koopa::Int;
}

koopa::Base *ast::FuncDef::to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const {
    auto func_id = new std::string("");
    *func_id += "@" + *id;
    return new koopa::FuncDef(
        value_saver.new_id(
            new koopa::FuncType(
                std::vector<koopa::Type *>(),
                static_cast<koopa::Type *>(func_type->to_koopa(value_saver, nesting_info))
            ),
            func_id,
            new ast::NestingInfo(false) /* This does not affect scope, since we only name zero-nested variables as unsuffixed */
        ),
        std::vector<koopa::FuncParamDecl *>(),
        static_cast<koopa::Type *>(func_type->to_koopa(value_saver, nesting_info)),
        static_cast<koopa_trans::Blocks *>(block->to_koopa(value_saver, nesting_info))->to_raw_blocks()
    );
}

koopa::Base *ast::CompUnit::to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const {
    return new koopa::Program(
        std::vector<koopa::GlobalStmt *> {
            static_cast<koopa::GlobalStmt *>(func_def->to_koopa(value_saver, new ast::NestingInfo()))
        }
    );
}