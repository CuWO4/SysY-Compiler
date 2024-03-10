#include "../include/ast.h"

#include <string>

namespace ast {

static const char *binary_op_name[] = {
    "||", "&&", "==", "!=", "<", ">", "<=", ">=", 
    "+", "-", "*", "/", "%", ",", "="
};

static const char *unary_op_name[] = { "-", "+", "!" };

std::string build_indent(int indent) {
    std::string res = "";
    for (int i = 0; i < indent; i++) res += "  ";
    return res;
}

std::string BinaryExpr::debug(int indent) const {
    return '(' + lv->debug() + ") " 
        + binary_op_name[op] 
        + " (" + rv->debug() + ')';
}

std::string UnaryExpr::debug(int indent) const {
    return std::string(unary_op_name[op]) 
        + '(' + lv->debug() + ')';
}

std::string Id::debug(int indent) const {
    return *lit + '(' + std::to_string(nesting_info->nesting_level) + ':' + std::to_string(nesting_info->nesting_count) + ')';
}

std::string FuncCall::debug(int indent) const {
    auto res = build_indent(indent) + func_id->debug();

    res += '(';
    for (auto actual_param : actual_params) {
        res += actual_param->debug() + ", ";
    }
    if (actual_params.size() > 0) {
        res.pop_back(); // `,`
        res.pop_back(); // ` `
    }
    res += ')';

    return res;
}

std::string Number::debug(int indent) const {
    return std::to_string(val);
}

std::string VarDef::debug(int indent) const {
    return id->debug() +
        (has_init ? " = " + init->debug() : "") + ", ";
}

std::string VarDecl::debug(int indent) const {
    auto res = build_indent(indent) + type->debug() + ' ';

    for (auto var_def : var_defs) {
        res += var_def->debug();
    }

    return res;
}

std::string Return::debug(int indent) const {
    return build_indent(indent) + "Return " + (has_return_val ? ret_val->debug() : "");
}

std::string If::debug(int indent) const {
    return build_indent(indent) + "If ( " + cond->debug() + " ) {\n"
        + then_stmt->debug(indent + 1) + '\n'
        + build_indent(indent) + "}"
        + (has_else_stmt 
            ? " Else {\n"
                + else_stmt->debug(indent + 1) + '\n'
                + build_indent(indent) + "}"
            : ""
        );
}

std::string While::debug(int indent) const {
    return build_indent(indent) + "While ( " + cond->debug() + " ) {\n"
        + body->debug(indent + 1) + '\n'
        + build_indent(indent) + "}";
}

std::string For::debug(int indent) const {
    return  build_indent(indent) + "For ( " + init_stmt->debug() + "; " + cond->debug() + "; " + iter_stmt->debug() + ") {\n"
        + body->debug(indent + 1) + '\n'
        + build_indent(indent) + "}";
}

std::string Continue::debug(int indent) const {
    return build_indent(indent) + "Continue";
}

std::string Break::debug(int indent) const {
    return build_indent(indent) + "Break";
}

std::string Block::debug(int indent) const {
    auto res = std::string("");

    for (auto stmt : stmts) {
        res += stmt->debug(indent) + '\n';
    }

    return res;
}

std::string Int::debug(int indent) const {
    return "Int";
}

std::string Void::debug(int indent) const {
    return "Void";
}

std::string FuncDef::debug(int indent) const {
    auto res = build_indent(indent) + ret_type->debug() + ' ' + *id->lit + '(';
    for (auto param : params) {
        res += std::get<0>(*param)->debug() + ' '
            + std::get<1>(*param)->debug() + ", ";
    }
    if (params.size() > 0) {
        res.pop_back(); // `,`
        res.pop_back(); // ` `
    }
    res += ")"; 
    res += "{\n" 
        + block->debug(indent + 1) 
        + build_indent(indent) + "}";
    return res;
}

std::string CompUnit::debug(int indent) const {
    std::string res = "";

    for (auto global_stmt : global_stmts) {
        res += global_stmt->debug() + '\n';
    }
    
    return res;
}

}