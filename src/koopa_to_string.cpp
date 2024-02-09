#include "../include/koopa.h"

#include <string>

std::string koopa::Int::to_string() {
    return "i32";
}

std::string koopa::Array::to_string() {
    return '[' + elem_type->to_string() + ',' 
            + std::to_string(length) + ']';
}

std::string koopa::Pointer::to_string() {
    return '*' + pointed_type->to_string();
}

std::string koopa::FuncType::to_string() {
    auto res = std::string("");

    res += '(';
    for (auto arg_type : *arg_types) {
        res += arg_type->to_string() + ',';
    }
    if (arg_types->size() > 0) res.pop_back();
    res += ")";

    if (!(typeid(ret_type) == typeid(koopa::Void))) {
        res += ": " + ret_type->to_string();
    }
    return res;
}

std::string koopa::Label::to_string() {
    return "label";
}

std::string koopa::Void::to_string() {
    return "void";
}

std::string koopa::Id::to_string() {
    auto res = std::string(lit != nullptr ? *lit : "");
    if ((typeid(type) == typeid(koopa::Int))
        || (typeid(type) == typeid(koopa::Array))
        || (typeid(type) == typeid(koopa::Pointer))) {
        res += "/*! type: " + type->to_string() + " */"; 
    }
    return res;
}

std::string koopa::Const::to_string() {
    return std::to_string(val);
}

std::string koopa::Undef::to_string() {
    return "undef";
}

std::string koopa::ConstInitializer::to_string() {
    return std::to_string(val);
}

std::string koopa::Aggregate::to_string() {
    auto res = std::string("");

    res += '{';
    for (auto initializer : *initializers) {
        res += initializer->to_string() + ',';
    }
    if (initializers->size() > 0) res.pop_back();
    res += '}';

    return res;
}

std::string koopa::Zeroinit::to_string() {
    return "zeroinit";
}

std::string koopa::UndefInitializer::to_string() {
    return "undef";
}

std::string koopa::MemoryDecl::to_string() {
    return "alloc\t" + type->to_string();
}

std::string koopa::Load::to_string() {
    return "load\t" + addr->to_string();
}

std::string koopa::GetPtr::to_string() {
    return "getptr\t" + base->to_string() + ',' + offset->to_string();
}

std::string koopa::GetElemPtr::to_string() {
    return "getelemptr\t" + base->to_string() + ',' + offset->to_string();
}

static std::string BINARY_OP_NAME[] = {
    "ne", "eq", "gt", "lt", "geq", "leq", "add", "sub", "mul",
    "div", "mod", "and", "or", "xor", "shl", "shr", "sar",
};

std::string koopa::Expr::to_string() {
    return BINARY_OP_NAME[op] + '	' + lv->to_string() + ',' + rv->to_string();
}

std::string koopa::FuncCall::to_string() {
    auto res = std::string("");

    res += "call\t" + id->to_string() + '(';
    for (auto arg : *args) {
        res += arg->to_string() + ',';
    }
    if (args->size() > 0) res.pop_back();
    res += ')';

    return res;
}

std::string koopa::SymbolDef::to_string() {
    return id->to_string() + '=' +val->to_string();
}

std::string koopa::StoreValue::to_string() {
    return "store\t" + value->to_string() + ',' + addr->to_string();
}

std::string koopa::StoreInitializer::to_string() {
    return "store\t" + initializer->to_string() + ',' + addr->to_string();
}

std::string koopa::Branch::to_string() {
    return "br\t" + cond->to_string() + ',' 
        + target1->to_string() + ',' + target2->to_string();
}

std::string koopa::Jump::to_string() {
    return "jump\t" + target->to_string();
}

std::string koopa::Return::to_string() {
    return "ret\t" + val->to_string();
}

std::string koopa::Block::to_string() {
    auto res = std::string("");

    if (preds.size() > 0) {
        res += "//! pred: ";
        for (auto pred : preds) res += pred + ',';
        res.pop_back();
        res += '\n';
    }
    if (succs.size() > 0) {
        res += "//! succ: ";
        for (auto succ : succs) res += succ + ',';
        res.pop_back();
        res += '\n';
    }

    res += id->to_string() + ":\n";
    for (auto stmt : *stmts) {
        res += '\t' + stmt->to_string() + '\n';
    }
    res += '\t' + end_stmt->to_string() + '\n';
    return res;
}

std::string koopa::FuncParamDecl::to_string() {
    return id->to_string() + ": " + type->to_string();
}

std::string koopa::FuncDef::to_string() {
    auto res = std::string("");

    res += "//! type: " + id->type->to_string() + '\n';

    res += "fun\t" + id->to_string();

    res += '(';
    for (auto func_param_decl : *func_param_decls) {
        res += func_param_decl->to_string() + ',';
    }
    if (func_param_decls->size() > 0) res.pop_back();
    res += ')';

    if (!(typeid(ret_type) == typeid(koopa::Void))) {
        res += ": " + ret_type->to_string();
    }
    
    for (auto block : *blocks) {
        res += "{\n";
        res += block->to_string();
        res += "}\n";
    }

    return res;
}

std::string koopa::FuncDecl::to_string() {
    auto res = std::string("");

    res += "//! type: " + id->type->to_string() + '\n';

    res += "decl\t";

    res += id->to_string();

    res += '(';
    for (auto param_type : *param_types) {
        res += param_type->to_string() + ',';
    }
    if (param_types->size() > 0) res.pop_back();
    res += ')';

    if (!(typeid(ret_type) == typeid(koopa::Void))) {
        res += ": " + ret_type->to_string();
    }

    return res;
}

std::string koopa::GlobalMemoryDecl::to_string() {
    return "alloc\t" + type->to_string() + ',' + initializer->to_string();
}

std::string koopa::GlobalSymbolDef::to_string() {
    return "global\t" + id->to_string() + '=' + decl->to_string();
}

std::string koopa::Program::to_string() {
    auto res = std::string("");

    for (auto global_stmt : *global_stmts) {
        res += global_stmt->to_string() + '\n';
    }

    return res;
}