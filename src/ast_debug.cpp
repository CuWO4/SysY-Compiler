#include "../include/ast.h"

#include <string>

namespace ast {

static const char *binary_op_name[] = {
    "||", "&&", "==", "!=", "<", ">", "<=", ">=", 
    "+", "-", "*", "/", "%",
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

std::string Number::debug(int indent) const {
    return std::to_string(val);
}

std::string ExprStmt::debug(int indent) const {
    return build_indent(indent) + expr->debug() + '\n';
}

std::string VarDef::debug(int indent) const {
    return id->debug() +
        (has_init ? " = " + init->debug() : "") + ',';
}

std::string VarDecl::debug(int indent) const {
    auto res = build_indent(indent) + type->debug() + ' ';

    for (auto var_def : var_defs) {
        res += var_def->debug();
    }

    res += '\n';

    return res;
}

std::string Assign::debug(int indent) const {
    return build_indent(indent) + id->debug() + " = " + rval->debug() + '\n';
}

std::string Return::debug(int indent) const {
    return build_indent(indent) + "Return " + (has_return_val ? ret_val->debug() : "") + '\n';
}

std::string If::debug(int indent) const {
    return build_indent(indent) + "If ( " + cond->debug() + " ) {\n"
        + then_stmt->debug(indent + 1)
        + build_indent(indent) + "}"
        + (has_else_stmt 
            ? " Else {\n"
                + else_stmt->debug(indent + 1)
                + build_indent(indent) + "}\n"
            : "\n"
        );
}

std::string Block::debug(int indent) const {
    auto res = std::string("");

    for (auto stmt : stmts) {
        res += stmt->debug(indent);
    }

    return res;
}

std::string Int::debug(int indent) const {
    return "Int";
}

std::string FuncDef::debug(int indent) const {
    return build_indent(indent) + func_type->debug() + ' ' + *id + "() {\n" 
        + block->debug(indent + 1) 
        + build_indent(indent) + "}";
}

std::string CompUnit::debug(int indent) const {
    return func_def->debug(indent);
}

}