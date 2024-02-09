#include "../include/ast.h"
#include "../include/koopa.h"

koopa::Base *ast::Return::to_koopa() const {
    return new koopa::Return(
        new koopa::Const(return_val)
    );
}

koopa::Base *ast::Block::to_koopa() const {
    return new koopa::Block(
        new koopa::Id(new koopa::Label, new std::string("%entry")),
        new std::vector<koopa::Stmt *>(),
        (koopa::EndStmt *)stmt->to_koopa()
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
                (koopa::Type *) func_type->to_koopa()
            ),
            func_id
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