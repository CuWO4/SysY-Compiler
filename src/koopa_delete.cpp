#include "../include/koopa.h"

koopa::Array::~Array() {
    if (elem_type != nullptr) delete elem_type;
}

koopa::Pointer::~Pointer() {
    if (pointed_type != nullptr) delete pointed_type;
}

koopa::FuncType::~FuncType() {
    for (auto arg_type : arg_types) {
        if (arg_type != nullptr) delete arg_type;
    }

    if (ret_type != nullptr) delete ret_type;

}

koopa::Id::~Id() {
    if (type != nullptr) delete type;
    if (lit != nullptr) delete lit;
}

koopa::Aggregate::~Aggregate() {
    for (auto initializer : initializers) {
        if (initializer != nullptr) delete initializer;
    }
}

koopa::MemoryDecl::~MemoryDecl() {
    if (type != nullptr) delete type;
}

koopa::Load::~Load() {
}

koopa::GetPtr::~GetPtr() {
}

koopa::GetElemPtr::~GetElemPtr() {
}

koopa::Expr::~Expr() {
}

koopa::ExprStmt::~ExprStmt() {
    if (expr != nullptr) delete expr;
}

koopa::FuncCall::~FuncCall() {
}

koopa::SymbolDef::~SymbolDef() {
}

koopa::StoreValue::~StoreValue() {
}

koopa::StoreInitializer::~StoreInitializer() {
    if (initializer != nullptr) delete initializer;
}

koopa::Branch::~Branch() {
}

koopa::Jump::~Jump() {
}

koopa::Return::~Return() {
}

koopa::Block::~Block() {
    for (auto stmt : stmts) {
        if (stmt != nullptr) delete stmt;
    }
}

koopa::FuncParamDecl::~FuncParamDecl() {
    if (type != nullptr) delete type;
}

koopa::FuncDef::~FuncDef() {
    for (auto func_param_decl : func_param_decls) {
        if (func_param_decl != nullptr) delete func_param_decl;
    }
    if (ret_type != nullptr) delete ret_type;
    for (auto block : blocks) {
        if (block != nullptr) delete block;
    }
}

koopa::FuncDecl::~FuncDecl() {
    for (auto param_type : param_types) {
        if (param_type != nullptr) delete param_type;
    }
    if (ret_type != nullptr) delete ret_type;
}

koopa::GlobalMemoryDecl::~GlobalMemoryDecl() {
    if (type != nullptr) delete type;
    if (initializer != nullptr) delete initializer;
}

koopa::GlobalSymbolDef::~GlobalSymbolDef() {
    if (decl != nullptr) delete decl;
}

koopa::Program::~Program() {
    for (auto global_stmt : global_stmts) {
        if (global_stmt != nullptr) delete global_stmt;
    }
}

