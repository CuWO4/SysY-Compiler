#include "../include/ast.h"

namespace ast {

BinaryExpr::~BinaryExpr() {
    if (lv != nullptr) delete lv;
    if (rv != nullptr) delete rv;
}

UnaryExpr::~UnaryExpr() {
    if (lv != nullptr) delete lv;
}

FuncCall::~FuncCall() {
    if (func_id != nullptr) delete func_id;
    for (auto actual_param : actual_params) 
        if (actual_param != nullptr) delete actual_param;
}

Id::~Id() {
    if (lit != nullptr) delete lit;
}

VarDef::~VarDef() {
    if (id != nullptr) delete id;
    if (init != nullptr) delete init;
}

VarDecl::~VarDecl() {
    if (type != nullptr) delete type;
    for (auto var_def : var_defs) {
        if (var_def != nullptr) delete var_def;
    }
}

Return::~Return() {
    if (ret_val != nullptr) delete ret_val;
}

If::~If() {
    if (cond != nullptr) delete cond;
    if (then_stmt != nullptr) delete then_stmt;
    if (else_stmt != nullptr) delete else_stmt;
}

While::~While() {
    if (cond != nullptr) delete cond;
    if (body != nullptr) delete body;
}

For::~For() {
    if (init_stmt != nullptr) delete init_stmt;
    if (cond != nullptr) delete cond;
    if (iter_stmt != nullptr) delete iter_stmt;
    if (body != nullptr) delete body;
}

Block::~Block() {
    for (auto stmt : stmts) {
        if (stmt != nullptr) delete stmt;
    }
}

FuncDef::~FuncDef() {
    if (ret_type != nullptr) delete ret_type;
    if (id != nullptr) delete id;
    if (block != nullptr) delete block;
}

GlobalVarDef::~GlobalVarDef() {
    if (id != nullptr) delete id;
    if (init != nullptr) delete init;
}

GlobalVarDecl::~GlobalVarDecl() {
    if (type != nullptr) delete type;
    for (auto var_def : var_defs) {
        if (var_def != nullptr) delete var_def;
    }
}

CompUnit::~CompUnit() {
    for (auto global_stmt : global_stmts)
        if (global_stmt != nullptr) delete global_stmt;
}

}