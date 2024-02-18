#include "../include/koopa.h"
#include "../include/assign.h"

#include <string>

std::string koopa::Int::to_string() const {
    return "i32";
}

std::string koopa::Array::to_string() const {
    return '[' + elem_type->to_string() + ',' 
            + std::to_string(length) + ']';
}

std::string koopa::Pointer::to_string() const {
    return '*' + pointed_type->to_string();
}

std::string koopa::FuncType::to_string() const {
    auto res = std::string("");

    res += '(';
    for (auto arg_type : arg_types) {
        res += arg_type->to_string() + ',';
    }
    if (arg_types.size() > 0) res.pop_back();
    res += ")";

    if (!(typeid(ret_type) == typeid(koopa::Void))) {
        res += ": " + ret_type->to_string();
    }
    return res;
}

std::string koopa::Label::to_string() const {
    return "label";
}

std::string koopa::Void::to_string() const {
    return "void";
}

std::string koopa::Id::to_string() const {
    auto res = std::string(lit != nullptr ? *lit : "");
    // if ((typeid(*type) == typeid(koopa::Int))
    //     || (typeid(*type) == typeid(koopa::Array))
    //     || (typeid(*type) == typeid(koopa::Pointer))) {
    //     res += " /*! type: " + type->to_string() + " */"; 
    // }
    return res;
}

std::string koopa::Const::to_string() const {
    return std::to_string(val);
}

std::string koopa::Undef::to_string() const {
    return "undef";
}

std::string koopa::ConstInitializer::to_string() const {
    return std::to_string(val);
}

std::string koopa::Aggregate::to_string() const {
    auto res = std::string("");

    res += '{';
    for (auto initializer : initializers) {
        res += initializer->to_string() + ',';
    }
    if (initializers.size() > 0) res.pop_back();
    res += '}';

    return res;
}

std::string koopa::Zeroinit::to_string() const {
    return "zeroinit";
}

std::string koopa::UndefInitializer::to_string() const {
    return "undef";
}

std::string koopa::MemoryDecl::to_string() const {
    return assign("alloc") + type->to_string();
}

std::string koopa::Load::to_string() const {
    return assign("load") + addr->to_string();
}

std::string koopa::GetPtr::to_string() const {
    return assign("getptr") + base->to_string() + ',' + offset->to_string();
}

std::string koopa::GetElemPtr::to_string() const {
    return assign("getelemptr", 12) + base->to_string() + ',' + offset->to_string();
}

static std::string BINARY_OP_NAME[] = {
    "ne", "eq", "gt", "lt", "ge", "le", "add", "sub", "mul",
    "div", "mod", "and", "or", "xor", "shl", "shr", "sar",
};

std::string koopa::Expr::to_string() const {
    return assign(BINARY_OP_NAME[op]) + lv->to_string() + ", " + rv->to_string();
}

std::string koopa::FuncCall::to_string() const {
    auto res = std::string("");

    res += assign("call") + id->to_string() + '(';
    for (auto arg : args) {
        res += arg->to_string() + ',';
    }
    if (args.size() > 0) res.pop_back();
    res += ')';

    return res;
}

std::string koopa::SymbolDef::to_string() const {
    return id->to_string() + "\t= " + val->to_string();
}

std::string koopa::StoreValue::to_string() const {
    return assign("store") + value->to_string() + ", " + addr->to_string();
}

std::string koopa::StoreInitializer::to_string() const {
    return assign("store") + initializer->to_string() + ", " + addr->to_string();
}

std::string koopa::Branch::to_string() const {
    return assign("br") + cond->to_string() + ',' 
        + target1->to_string() + ',' + target2->to_string();
}

std::string koopa::Jump::to_string() const {
    return assign("jump") + target->to_string();
}

std::string koopa::Return::to_string() const {
    return assign("ret") + val->to_string();
}

std::string koopa::Block::to_string() const {
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
    for (auto stmt : stmts) {
        res += '\t' + stmt->to_string() + '\n';
    }
    return res;
}

std::string koopa::FuncParamDecl::to_string() const {
    return id->to_string() + ": " + type->to_string();
}

std::string koopa::FuncDef::to_string() const {
    auto res = std::string("");

    res += "//! type: " + id->type->to_string() + '\n';

    res += "fun " + id->to_string();

    res += '(';
    for (auto func_param_decl : func_param_decls) {
        res += func_param_decl->to_string() + ',';
    }
    if (func_param_decls.size() > 0) res.pop_back();
    res += ')';

    if (!(typeid(ret_type) == typeid(koopa::Void))) {
        res += ": " + ret_type->to_string();
    }
    
    for (auto block : blocks) {
        res += "{\n";
        res += block->to_string();
        res += "}\n";
    }

    return res;
}

std::string koopa::FuncDecl::to_string() const {
    auto res = std::string("");

    res += "//! type: " + id->type->to_string() + '\n';

    res += assign("decl");

    res += id->to_string();

    res += '(';
    for (auto param_type : param_types) {
        res += param_type->to_string() + ',';
    }
    if (param_types.size() > 0) res.pop_back();
    res += ')';

    if (!(typeid(ret_type) == typeid(koopa::Void))) {
        res += ": " + ret_type->to_string();
    }

    return res;
}

std::string koopa::GlobalMemoryDecl::to_string() const {
    return assign("alloc") + type->to_string() + ',' + initializer->to_string();
}

std::string koopa::GlobalSymbolDef::to_string() const {
    return assign("global") + id->to_string() + '=' + decl->to_string();
}

std::string koopa::Program::to_string() const {
    auto res = std::string("");

    for (auto global_stmt : global_stmts) {
        res += global_stmt->to_string() + '\n';
    }

    return res;
}