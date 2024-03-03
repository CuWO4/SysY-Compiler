#include "../include/koopa.h"
#include "../include/def.h"

#include <string>

namespace koopa {

std::string Int::to_string() const {
    return "i32";
}

std::string Array::to_string() const {
    return '[' + elem_type->to_string() + ", " 
            + std::to_string(length) + ']';
}

std::string Pointer::to_string() const {
    return '*' + pointed_type->to_string();
}

std::string FuncType::to_string() const {
    auto res = std::string("");

    res += '(';
    for (auto arg_type : arg_types) {
        res += arg_type->to_string() + ", ";
    }
    if (arg_types.size() > 0) res.pop_back();
    res += ")";

    if (!(typeid(ret_type) == typeid(Void))) {
        res += ": " + ret_type->to_string();
    }
    return res;
}

std::string Label::to_string() const {
    return "label";
}

std::string Void::to_string() const {
    return "void";
}

std::string Id::to_string() const {
    auto res = *lit;
    if (debug_mode_koopa_type) {
        if ((typeid(*type) == typeid(Int))
            || (typeid(*type) == typeid(Array))
            || (typeid(*type) == typeid(Pointer))) {
            res += " /*! type: " + type->to_string() + " */"; 
        }
    }
    return res;
}

std::string Const::to_string() const {
    return std::to_string(val);
}

std::string Undef::to_string() const {
    return "undef";
}

std::string ConstInitializer::to_string() const {
    return std::to_string(val);
}

std::string Aggregate::to_string() const {
    auto res = std::string("");

    res += '{';
    for (auto initializer : initializers) {
        res += initializer->to_string() + ", ";
    }
    if (initializers.size() > 0) res.pop_back();
    res += '}';

    return res;
}

std::string Zeroinit::to_string() const {
    return "zeroinit";
}

std::string UndefInitializer::to_string() const {
    return "undef";
}

std::string MemoryDecl::to_string() const {
    return "alloc " + type->to_string();
}

std::string Load::to_string() const {
    return "load " + addr->to_string();
}

std::string GetPtr::to_string() const {
    return "getptr " + base->to_string() + ", " + offset->to_string();
}

std::string GetElemPtr::to_string() const {
    return "getelemptr " + base->to_string() + ", " + offset->to_string();
}

static std::string BINARY_OP_NAME[] = {
    "ne", "eq", "gt", "lt", "ge", "le", "add", "sub", "mul",
    "div", "mod", "and", "or", "xor", "shl", "shr", "sar",
};

std::string Expr::to_string() const {
    return BINARY_OP_NAME[op] + ' ' + lv->to_string() + ", " + rv->to_string();
}

std::string ExprStmt::to_string() const {
    return expr->to_string();
}

std::string FuncCall::to_string() const {
    auto res = std::string("");

    res += "call " + id->to_string() + '(';
    for (auto arg : args) {
        res += arg->to_string() + ", ";
    }
    if (args.size() > 0) res.pop_back();
    res += ')';

    return res;
}

std::string SymbolDef::to_string() const {
    return id->to_string() + " = " + val->to_string();
}

std::string StoreValue::to_string() const {
    return "store " + value->to_string() + ", " + addr->to_string();
}

std::string StoreInitializer::to_string() const {
    return "store " + initializer->to_string() + ", " + addr->to_string();
}

std::string Branch::to_string() const {
    return "br " + cond->to_string() + ", " 
        + target1->to_string() + ", " + target2->to_string();
}

std::string Jump::to_string() const {
    return "jump " + target->to_string();
}

std::string Return::to_string() const {
    return "ret " + (has_return_val ? val->to_string() : "");
}

std::string Block::to_string() const {
    auto res = std::string("");

    if (debug_mode_koopa_pred_succ) {
        if (preds.size() > 0) {
            res += "//! pred: ";
            for (auto pred : preds) res += pred + ", ";
            res.pop_back();
            res += '\n';
        }
        if (succs.size() > 0) {
            res += "//! succ: ";
            for (auto succ : succs) res += succ + ", ";
            res.pop_back();
            res += '\n';
        }
    }

    res += id->to_string() + ":\n";
    for (auto stmt : stmts) {
        res += '\t' + stmt->to_string() + '\n';
    }
    return res;
}

std::string FuncParamDecl::to_string() const {
    return id->to_string() + ": " + type->to_string();
}

std::string FuncDef::to_string() const {
    auto res = std::string("");

    if (debug_mode_koopa_type) res += "//! type: " + id->type->to_string() + '\n';

    res += "fun " + id->to_string();

    res += '(';
    for (auto func_param_decl : func_param_decls) {
        res += func_param_decl->to_string() + ", ";
    }
    if (func_param_decls.size() > 0) res.pop_back();
    res += ')';

    if (!(typeid(ret_type) == typeid(Void))) {
        res += ": " + ret_type->to_string();
    }
    
    res += " {\n";
    for (auto block : blocks) {
        res += block->to_string() + '\n';
    }
    res += "}\n";

    return res;
}

std::string FuncDecl::to_string() const {
    auto res = std::string("");

    if (debug_mode_koopa_type) res += "//! type: " + id->type->to_string() + '\n';

    res += "decl ";

    res += id->to_string();

    res += '(';
    for (auto param_type : param_types) {
        res += param_type->to_string() + ", ";
    }
    if (param_types.size() > 0) res.pop_back();
    res += ')';

    if (!(typeid(ret_type) == typeid(Void))) {
        res += ": " + ret_type->to_string();
    }

    return res;
}

std::string GlobalMemoryDecl::to_string() const {
    return "alloc " + type->to_string() + ", " + initializer->to_string();
}

std::string GlobalSymbolDef::to_string() const {
    return "global " + id->to_string() + '=' + decl->to_string();
}

std::string Program::to_string() const {
    auto res = std::string("");

    for (auto global_stmt : global_stmts) {
        res += global_stmt->to_string() + '\n';
    }

    return res;
}

}