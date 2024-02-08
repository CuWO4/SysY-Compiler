#include "../include/ast.h"
#include "../include/koopa.h"

koopa::Base *ast::Return::to_koopa() const {
    return new koopa::Return(
        new koopa::Const(return_val)
    );
}

koopa::Base *ast::Block::to_koopa() const {
    return new koopa::Block(
        new std::vector<koopa::Stmt *>(),
        (koopa::EndStmt *)stmt->to_koopa()
    );
}

koopa::Base *ast::IntFuncType::to_koopa() const {
    return new koopa::FuncType(
        new std::vector<koopa::Type *>(),
        new koopa::Int
    );
}

koopa::Base *ast::FuncDef::to_koopa() const {
    return new koopa::FuncDef(
        new koopa::Id(
            (koopa::Type *) func_type->to_koopa(),
            id
        ),
        new std::vector<koopa::FuncParamDecl *>(),
        (koopa::Type *) func_type->to_koopa(),
        new std::vector<koopa::Block *> {
            (koopa::Block *) block->to_koopa()
        }
    );
}

koopa::Base *ast::CompUnit::to_koopa() const {
    return new koopa::Program(
        new std::vector<koopa::GlobalStmt *> {
            (koopa::GlobalStmt *) func_def->to_koopa()
        }
    );
}