#include "../include/ast.h"
#include "../include/koopa.h"

#include <string>

namespace koopa_trans {
    class Stmts : public koopa::Base {
    public:
        std::vector<koopa::Stmt *> stmts = {};
        koopa::Value *last_val = nullptr;

        std::string to_string() const { return ""; }
        void to_riscv(std::string &str, riscv_trans::Info &info) const {}
    };

    void merge(Stmts &res, Stmts &a, Stmts &b) {
        auto &res_vec = res.stmts;
        auto &a_vec = a.stmts;
        auto &b_vec = b.stmts;
        res_vec.clear();
        res_vec.reserve(a_vec.size() + b_vec.size() + 5);
        res_vec.insert(res_vec.end(), a_vec.begin(), a_vec.end());
        res_vec.insert(res_vec.end(), b_vec.begin(), b_vec.end());

        res.last_val = b.last_val;
    }

    void operator+=(Stmts &res, Stmts x) {
        auto &res_vec = res.stmts;
        auto &x_vec = x.stmts;
        res_vec.insert(res_vec.end(), x_vec.begin(), x_vec.end());
    }
}

static int tmp_var_count = 0;

koopa::Base *ast::BinaryExpr::to_koopa(koopa::ValueSaver &value_saver) const {
    auto res = new koopa_trans::Stmts;

    auto lv_stmts = static_cast<koopa_trans::Stmts *>(lv->to_koopa(value_saver));
    auto rv_stmts = static_cast<koopa_trans::Stmts *>(rv->to_koopa(value_saver));

    koopa_trans::merge(*res, *lv_stmts, *rv_stmts);

    auto generator = [&] (koopa::op::Op op, koopa::Value *elv, koopa::Value *erv) {
        if (elv->is_const && erv->is_const) {
            res->stmts.clear();

            res->last_val = value_saver.new_const(
                koopa::op::op_func[op](elv->val, erv->val)
            );

            return;
        }
            
        res->last_val = value_saver.new_id(
            new koopa::Int,
            new std::string('%' + std::to_string(tmp_var_count++))
        );
        
        res->stmts.push_back( 
            new koopa::SymbolDef(
                static_cast<koopa::Id *>(res->last_val),
                new koopa::Expr(
                    op,
                    elv,
                    erv
                )
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

koopa::Base *ast::UnaryExpr::to_koopa(koopa::ValueSaver &value_saver) const {
    auto lv_stmts = static_cast<koopa_trans::Stmts *>(lv->to_koopa(value_saver));
    
    auto generator = [&](koopa::op::Op op) {
        if (lv_stmts->last_val->is_const) {
            lv_stmts->stmts.clear();

            lv_stmts->last_val = value_saver.new_const(koopa::op::op_func[op](0, lv_stmts->last_val->val));

            return;
        }

        if (op == koopa::op::ADD) return;

        koopa::NotEndStmt *new_stmt;
        koopa::Id *new_id;

        new_id = value_saver.new_id(
            new koopa::Int,
            new std::string('%' + std::to_string(tmp_var_count++))
        );

        new_stmt = new koopa::SymbolDef(
            new_id,
            new koopa::Expr(
                op,
                value_saver.new_const(0),
                lv_stmts->last_val
            )
        );

        lv_stmts->stmts.push_back(new_stmt);   
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

koopa::Base *ast::Number::to_koopa(koopa::ValueSaver &value_saver) const {
    auto res = new koopa_trans::Stmts;
    res->last_val = value_saver.new_const(val);
    return res;
}

koopa::Base *ast::Id::to_koopa(koopa::ValueSaver &value_saver) const {
    auto res = new koopa_trans::Stmts;

    auto id_koopa = value_saver.get_id('@' + *lit);

    if (id_koopa == nullptr) {
        throw "unknown identifier `" + *lit + '`';
    }

    if (id_koopa->is_const) {

        res->last_val = value_saver.new_const(id_koopa->val);
        return res;

    }
    else {

        auto new_id = new koopa::Id(
            static_cast<koopa::Pointer *>(id_koopa->type)->pointed_type, //! re-free bug
            new std::string('%' + std::to_string(tmp_var_count++))
        );

        res->stmts.push_back(
            new koopa::SymbolDef(
                new_id,
                new koopa::Load(id_koopa)
            )
        );

        res->last_val = new_id;

        return res;
        
    }
}

koopa::Base *ast::VarDecl::to_koopa(koopa::ValueSaver &value_saver) const {
    auto stmts = new koopa_trans::Stmts();

    if (is_const) {

        for (auto var_def : var_defs) {
            if (value_saver.get_id('@' + *var_def->id->lit) != nullptr) {
                throw '`' + *var_def->id->lit + "` redefined";
            }

            if (!var_def->has_init) {
                throw "no initiator for const variable `" + *var_def->id->lit + '`';
            }

            auto rval_koopa = static_cast<koopa_trans::Stmts *>(var_def->init->to_koopa(value_saver));

            if (!rval_koopa->last_val->is_const) {
                throw "initiating const variable `" + *var_def->id->lit + "` with a non-const value";
            }

            value_saver.new_id(
                static_cast<koopa::Type *>(type->to_koopa(value_saver)), 
                new std::string('@' + *var_def->id->lit),
                true,
                rval_koopa->last_val->val
            );

            for (auto stmt : stmts->stmts) if (stmt != nullptr) delete stmt;
            delete stmts;
        }

    }

    else {

        for (auto var_def : var_defs) {
            if (value_saver.get_id('@' + *var_def->id->lit) != nullptr) {
                throw '`' + *var_def->id->lit + "` redefined";
            }

            stmts->stmts.push_back(
                new koopa::SymbolDef(
                    value_saver.new_id(
                        new koopa::Pointer(
                            static_cast<koopa::Type *>(type->to_koopa(value_saver))
                        ),
                        new std::string('@' + *var_def->id->lit)
                    ),
                    new koopa::MemoryDecl(static_cast<koopa::Type *>(type->to_koopa(value_saver)))
                )
            );

            if (var_def->has_init) {

                auto rval_koopa = static_cast<koopa_trans::Stmts *>(var_def->init->to_koopa(value_saver));

                *stmts += *rval_koopa;

                stmts->stmts.push_back(
                    new koopa::StoreValue(
                        rval_koopa->last_val,
                        value_saver.get_id('@' + *var_def->id->lit)
                    )
                );
            }
        }

    }

    return stmts;
}

koopa::Base *ast::Assign::to_koopa(koopa::ValueSaver &value_saver) const {
    auto res = static_cast<koopa_trans::Stmts *>(rval->to_koopa(value_saver));

    res->stmts.push_back(
        new koopa::StoreValue(
            res->last_val,
            value_saver.get_id('@' + *id->lit)
        )
    );

    return res;
}

koopa::Base *ast::Return::to_koopa(koopa::ValueSaver &value_saver) const {
    auto res = static_cast<koopa_trans::Stmts *>(ret_val->to_koopa(value_saver));
    
    res->stmts.push_back(
        new koopa::Return(
            res->last_val
        )
    );
    
    return res;
}

koopa::Base *ast::Block::to_koopa(koopa::ValueSaver &value_saver) const {
    auto res = koopa_trans::Stmts();

    for (auto stmt : stmts) {
        res += *static_cast<koopa_trans::Stmts *>(stmt->to_koopa(value_saver));
    }

    return new koopa::Block(
        value_saver.new_id(new koopa::Label, new std::string("%entry")),
        res.stmts
    );
}

koopa::Base *ast::Int::to_koopa(koopa::ValueSaver &value_saver) const {
    return new koopa::Int;
}

koopa::Base *ast::FuncDef::to_koopa(koopa::ValueSaver &value_saver) const {
    auto func_id = new std::string("");
    *func_id += "@" + *id;
    return new koopa::FuncDef(
        value_saver.new_id(
            new koopa::FuncType(
                std::vector<koopa::Type *>(), 
                static_cast<koopa::Type *>(func_type->to_koopa(value_saver))
            ),
            func_id
        ),
        std::vector<koopa::FuncParamDecl *>(),
        static_cast<koopa::Type *>(func_type->to_koopa(value_saver)),
        std::vector<koopa::Block *> {
            static_cast<koopa::Block *>(block->to_koopa(value_saver))
        }
    );
}

koopa::Base *ast::CompUnit::to_koopa(koopa::ValueSaver &value_saver) const {
    return new koopa::Program(
        std::vector<koopa::GlobalStmt *> {
            static_cast<koopa::GlobalStmt *>(func_def->to_koopa(value_saver))
        }
    );
}