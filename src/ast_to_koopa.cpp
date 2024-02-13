#include "../include/ast.h"
#include "../include/koopa.h"

#include <string>

namespace koopa_trans {
    class Stmts : public koopa::Base {
    public:
        std::vector<koopa::Stmt *> stmts = {};
        koopa::Value *last_val = nullptr;

        std::string to_string() const { return ""; }
        std::string to_riscv() const { return ""; }
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
}

static int tmp_var_count = 0;

koopa::Base *ast::BinaryExpr::to_koopa(koopa::ValueSaver &value_saver) const {
    auto res = new koopa_trans::Stmts;

    auto lv_stmts = static_cast<koopa_trans::Stmts *>(lv->to_koopa(value_saver));
    auto rv_stmts = static_cast<koopa_trans::Stmts *>(rv->to_koopa(value_saver));

    koopa_trans::merge(*res, *lv_stmts, *rv_stmts);

    koopa::NotEndStmt *new_stmt;

    auto generator = [&] (koopa::op::Op op, koopa::Value *elv, koopa::Value *erv) -> koopa::SymbolDef * {
            
        res->last_val = value_saver.new_id(
            new koopa::Int,
            new std::string('%' + std::to_string(tmp_var_count++))
        );
        
        return new koopa::SymbolDef(
            static_cast<koopa::Id *>(res->last_val),
            new koopa::Expr(
                op,
                elv,
                erv
            )
        );
    };

    switch (op) {
        case op::LOGIC_AND: {
            res->stmts.push_back(generator(koopa::op::NE, lv_stmts->last_val, value_saver.new_const(0)));
            auto tmpa = res->last_val;
            res->stmts.push_back(generator(koopa::op::NE, rv_stmts->last_val, value_saver.new_const(0)));
            auto tmpb = res->last_val;
            new_stmt = generator(koopa::op::AND, tmpa, tmpb);
            break;
        }
        case op::LOGIC_OR: {
            res->stmts.push_back(generator(koopa::op::NE, lv_stmts->last_val, value_saver.new_const(0)));
            auto tmpa = res->last_val;
            res->stmts.push_back(generator(koopa::op::NE, rv_stmts->last_val, value_saver.new_const(0)));
            auto tmpb = res->last_val;
            new_stmt = generator(koopa::op::OR, tmpa, tmpb);
            break;
        }
        case op::EQ: new_stmt = generator(koopa::op::EQ, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::NEQ: new_stmt = generator(koopa::op::NE, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::LT: new_stmt = generator(koopa::op::LT, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::GT: new_stmt = generator(koopa::op::GT, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::LEQ: new_stmt = generator(koopa::op::LE, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::GEQ: new_stmt = generator(koopa::op::GE, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::ADD: new_stmt = generator(koopa::op::ADD, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::SUB: new_stmt = generator(koopa::op::SUB, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::MUL: new_stmt = generator(koopa::op::MUL, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::DIV: new_stmt = generator(koopa::op::DIV, lv_stmts->last_val, rv_stmts->last_val); break;
        case op::MOD: new_stmt = generator(koopa::op::MOD, lv_stmts->last_val, rv_stmts->last_val); break;
    }

    res->stmts.push_back(new_stmt);

    delete lv_stmts;
    delete rv_stmts;

    return res;
}

koopa::Base *ast::UnaryExpr::to_koopa(koopa::ValueSaver &value_saver) const {
    auto lv_stmts = static_cast<koopa_trans::Stmts *>(lv->to_koopa(value_saver));
    
    auto generator = [&](koopa::op::Op op) {
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
    auto stmts = static_cast<koopa_trans::Stmts *>(stmt->to_koopa(value_saver));

    return new koopa::Block(
        value_saver.new_id(new koopa::Label, new std::string("%entry")),
        stmts->stmts
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