#include "../include/koopa.h"

koopa::Array::~Array() {
    if (elem_type != nullptr) delete elem_type;
}

koopa::Pointer::~Pointer() {
    if (pointed_type != nullptr) delete pointed_type;
}

koopa::FuncType::~FuncType() {
    for (auto arg_type : *arg_types) {
        if (arg_type != nullptr) delete arg_type;
    }
    if (arg_types != nullptr) delete arg_types;
    if (ret_type != nullptr) delete ret_type;

}

koopa::Id::~Id() {
    if (type != nullptr) delete type;
    if (lit != nullptr) delete lit;
}

koopa::Aggregate::~Aggregate() {
    for (auto initializer : *initializers) {
        if (initializer != nullptr) delete initializer;
    }
    if (initializers != nullptr) delete initializers;
}

koopa::MemoryDecl::~MemoryDecl() {
    if (type != nullptr) delete type;
}

koopa::Load::~Load() {
    if (addr != nullptr) delete addr;
}

koopa::GetPtr::~GetPtr() {
    if (base != nullptr) delete base;
    if (offset != nullptr) delete offset;
}

koopa::GetElemPtr::~GetElemPtr() {
    if (base != nullptr) delete base;
    if (offset != nullptr) delete offset;
}

koopa::Expr::~Expr() {
    if (lv != nullptr) delete lv;
    if (rv != nullptr) delete rv;
}

koopa::FuncCall::~FuncCall() {
    if (id != nullptr) delete id;
    for (auto arg : *args) {
        if (arg != nullptr) delete arg;
    }
    if (args != nullptr) delete args;
}

koopa::SymbolDef::~SymbolDef() {
    if (id != nullptr) delete id;
    if (val != nullptr) delete val;
}

koopa::StoreValue::~StoreValue() {
    if (value != nullptr) delete value;
    if (addr != nullptr) delete addr;
}

koopa::StoreInitializer::~StoreInitializer() {
    if (initializer != nullptr) delete initializer;
    if (addr != nullptr) delete addr;
}

koopa::Branch::~Branch() {
    if (cond != nullptr) delete cond;
    if (target[0] != nullptr) delete target[0];
    if (target[1] != nullptr) delete target[1];
}

koopa::Jump::~Jump() {
    if (target != nullptr) delete target;
}

koopa::Return::~Return() {
    if (val != nullptr) delete val;
}

koopa::Block::~Block() {
    for (auto stmt : *stmts) {
        if (stmt != nullptr) delete stmt;
    }
    if (stmts != nullptr) delete stmts;
    if (end_stmt != nullptr) delete end_stmt;
}

koopa::FuncParamDecl::~FuncParamDecl() {
    if (id != nullptr) delete id;
    if (type != nullptr) delete type;
}

koopa::FuncDef::~FuncDef() {
    if (id != nullptr) delete id;
    for (auto func_param_decl : *func_param_decls) {
        if (func_param_decl != nullptr) delete func_param_decl;
    }
    if (func_param_decls != nullptr) delete func_param_decls;
    if (ret_type != nullptr) delete ret_type;
    for (auto block : *blocks) {
        if (block != nullptr) delete block;
    }
    if (blocks != nullptr) delete blocks;
}

koopa::FuncDecl::~FuncDecl() {
    if (id != nullptr) delete id;
    for (auto param_type : *param_types) {
        if (param_type != nullptr) delete param_type;
    }
    if (param_types != nullptr) delete param_types;
    if (ret_type != nullptr) delete ret_type;
}

koopa::GlobalMemoryDecl::~GlobalMemoryDecl() {
    if (type != nullptr) delete type;
    if (initializer != nullptr) delete initializer;
}

koopa::GlobalSymbolDef::~GlobalSymbolDef() {
    if (id != nullptr) delete id;
    if (decl != nullptr) delete decl;
}

koopa::Program::~Program() {
    for (auto global_stmt : *global_stmts) {
        if (global_stmt != nullptr) delete global_stmt;
    }
    if (global_stmts != nullptr) delete global_stmts;
}

