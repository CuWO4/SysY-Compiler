#include "../include/koopa.h"

namespace koopa {

Array::~Array() {
    if (elem_type != nullptr) delete elem_type;
}

Pointer::~Pointer() {
    if (pointed_type != nullptr) delete pointed_type;
}

FuncType::~FuncType() {
    for (auto arg_type : arg_types) {
        if (arg_type != nullptr) delete arg_type;
    }

    if (ret_type != nullptr) delete ret_type;

}

Id::~Id() {
    if (type != nullptr) delete type;
    if (lit != nullptr) delete lit;
}

Aggregate::~Aggregate() {
    for (auto initializer : initializers) {
        if (initializer != nullptr) delete initializer;
    }
}

MemoryDecl::~MemoryDecl() {
    if (type != nullptr) delete type;
}

Load::~Load() {
}

GetPtr::~GetPtr() {
}

GetElemPtr::~GetElemPtr() {
}

Expr::~Expr() {
}

FuncCall::~FuncCall() {
}

SymbolDef::~SymbolDef() {
}

StoreValue::~StoreValue() {
}

StoreInitializer::~StoreInitializer() {
    if (initializer != nullptr) delete initializer;
}

Branch::~Branch() {
}

Jump::~Jump() {
}

Return::~Return() {
}

Block::~Block() {
    for (auto stmt : stmts) {
        if (stmt != nullptr) delete stmt;
    }
}

FuncDef::~FuncDef() {
    for (auto formal_param_id: formal_param_ids) {
        if (formal_param_id != nullptr) delete formal_param_id;
    }
    if (ret_type != nullptr) delete ret_type;
    for (auto block : blocks) {
        if (block != nullptr) delete block;
    }
}

FuncDecl::~FuncDecl() {
    if (ret_type != nullptr) delete ret_type;
}

GlobalMemoryDecl::~GlobalMemoryDecl() {
    if (type != nullptr) delete type;
    if (initializer != nullptr) delete initializer;
}

GlobalSymbolDef::~GlobalSymbolDef() {
    if (decl != nullptr) delete decl;
}

Program::~Program() {
    for (auto global_stmt : global_stmts) {
        if (global_stmt != nullptr) delete global_stmt;
    }
}

}