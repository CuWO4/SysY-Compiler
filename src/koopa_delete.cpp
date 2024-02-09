#include "../include/koopa.h"

koopa::Array::~Array() {
    if (elem_type != nullptr) { delete elem_type; elem_type = nullptr;  }
}

koopa::Pointer::~Pointer() {
    if (pointed_type != nullptr) { delete pointed_type; pointed_type = nullptr;  }
}

koopa::FuncType::~FuncType() {
    for (auto arg_type : *arg_types) {
        if (arg_type != nullptr) { delete arg_type; arg_type = nullptr;  }
    }
    if (arg_types != nullptr) { delete arg_types; arg_types = nullptr;  }
    if (ret_type != nullptr) { delete ret_type; ret_type = nullptr;  }

}

koopa::Id::~Id() {
    if (type != nullptr) { delete type; type = nullptr;  }
    if (lit != nullptr) { delete lit; lit = nullptr;  }
}

koopa::Aggregate::~Aggregate() {
    for (auto initializer : *initializers) {
        if (initializer != nullptr) { delete initializer; initializer = nullptr;  }
    }
    if (initializers != nullptr) { delete initializers; initializers = nullptr;  }
}

koopa::MemoryDecl::~MemoryDecl() {
    if (type != nullptr) { delete type; type = nullptr;  }
}

koopa::Load::~Load() {
    if (addr != nullptr) { delete addr; addr = nullptr;  }
}

koopa::GetPtr::~GetPtr() {
    if (base != nullptr) { delete base; base = nullptr;  }
    if (offset != nullptr) { delete offset; offset = nullptr;  }
}

koopa::GetElemPtr::~GetElemPtr() {
    if (base != nullptr) { delete base; base = nullptr;  }
    if (offset != nullptr) { delete offset; offset = nullptr;  }
}

koopa::Expr::~Expr() {
    if (lv != nullptr) { delete lv; lv = nullptr;  }
    if (rv != nullptr) { delete rv; rv = nullptr;  }
}

koopa::FuncCall::~FuncCall() {
    if (id != nullptr) { delete id; id = nullptr;  }
    for (auto arg : *args) {
        if (arg != nullptr) { delete arg; arg = nullptr;  }
    }
    if (args != nullptr) { delete args; args = nullptr;  }
}

koopa::SymbolDef::~SymbolDef() {
    if (id != nullptr) { delete id; id = nullptr;  }
    if (val != nullptr) { delete val; val = nullptr;  }
}

koopa::StoreValue::~StoreValue() {
    if (value != nullptr) { delete value; value = nullptr;  }
    if (addr != nullptr) { delete addr; addr = nullptr;  }
}

koopa::StoreInitializer::~StoreInitializer() {
    if (initializer != nullptr) { delete initializer; initializer = nullptr;  }
    if (addr != nullptr) { delete addr; addr = nullptr;  }
}

koopa::Branch::~Branch() {
    if (cond != nullptr) { delete cond; cond = nullptr;  }
    if (target1 != nullptr) { delete target1; target1 = nullptr;  }
    if (target2 != nullptr) { delete target2; target2 = nullptr;  }
}

koopa::Jump::~Jump() {
    if (target != nullptr) { delete target; target = nullptr;  }
}

koopa::Return::~Return() {
    if (val != nullptr) { delete val; val = nullptr;  }
}

koopa::Block::~Block() {
    if (id != nullptr) { delete id; id = nullptr;  }
    for (auto stmt : *stmts) {
        if (stmt != nullptr) { delete stmt; stmt = nullptr;  }
    }
    if (stmts != nullptr) { delete stmts; stmts = nullptr;  }
    if (end_stmt != nullptr) { delete end_stmt; end_stmt = nullptr;  }
}

koopa::FuncParamDecl::~FuncParamDecl() {
    if (id != nullptr) { delete id; id = nullptr;  }
    if (type != nullptr) { delete type; type = nullptr;  }
}

koopa::FuncDef::~FuncDef() {
    if (id != nullptr) { delete id; id = nullptr;  }
    for (auto func_param_decl : *func_param_decls) {
        if (func_param_decl != nullptr) { delete func_param_decl; func_param_decl = nullptr;  }
    }
    if (func_param_decls != nullptr) { delete func_param_decls; func_param_decls = nullptr;  }
    if (ret_type != nullptr) { delete ret_type; ret_type = nullptr;  }
    for (auto block : *blocks) {
        if (block != nullptr) { delete block; block = nullptr;  }
    }
    if (blocks != nullptr) { delete blocks; blocks = nullptr;  }
}

koopa::FuncDecl::~FuncDecl() {
    if (id != nullptr) { delete id; id = nullptr;  }
    for (auto param_type : *param_types) {
        if (param_type != nullptr) { delete param_type; param_type = nullptr;  }
    }
    if (param_types != nullptr) { delete param_types; param_types = nullptr;  }
    if (ret_type != nullptr) { delete ret_type; ret_type = nullptr;  }
}

koopa::GlobalMemoryDecl::~GlobalMemoryDecl() {
    if (type != nullptr) { delete type; type = nullptr;  }
    if (initializer != nullptr) { delete initializer; initializer = nullptr;  }
}

koopa::GlobalSymbolDef::~GlobalSymbolDef() {
    if (id != nullptr) { delete id; id = nullptr;  }
    if (decl != nullptr) { delete decl; decl = nullptr;  }
}

koopa::Program::~Program() {
    for (auto global_stmt : *global_stmts) {
        if (global_stmt != nullptr) { delete global_stmt; global_stmt = nullptr;  }
    }
    if (global_stmts != nullptr) { delete global_stmts; global_stmts = nullptr;  }
}

