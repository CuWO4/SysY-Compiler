#include "../include/ast.h"
#include "../include/koopa.h"

koopa::Base *ast::BinaryExpr::to_koopa() const {
    return new koopa::Const(0);
}

koopa::Base *ast::UnaryExpr::to_koopa() const {
    return new koopa::Const(0);
}

koopa::Base *ast::Number::to_koopa() const {
    return new koopa::Const(0);
}

koopa::Base *ast::Return::to_koopa() const {
    return new koopa::Return(
        new koopa::Const(ret_val->val)
    );
}

koopa::Base *ast::Block::to_koopa() const {
    return new koopa::Block(
        new koopa::Id(new koopa::Label, new std::string("%entry")),
        new std::vector<koopa::Stmt *>(),
        static_cast<koopa::EndStmt *>(stmt->to_koopa())
    );
}

koopa::Base *ast::Int::to_koopa() const {
    return new koopa::Int;
}

koopa::Base *ast::FuncDef::to_koopa() const {
    auto func_id = new std::string("");
    *func_id += "@" + *id;
    return new koopa::FuncDef(
        new koopa::Id(
            new koopa::FuncType(
                new std::vector<koopa::Type *>(), 
                static_cast<koopa::Type *>(func_type->to_koopa())
            ),
            func_id
        ),
        new std::vector<koopa::FuncParamDecl *>(),
        static_cast<koopa::Type *>(func_type->to_koopa()),
        new std::vector<koopa::Block *> {
            static_cast<koopa::Block *>(block->to_koopa())
        }
    );
}

koopa::Base *ast::CompUnit::to_koopa() const {
    return new koopa::Program(
        new std::vector<koopa::GlobalStmt *> {
            static_cast<koopa::GlobalStmt *>(func_def->to_koopa())
        }
    );
}