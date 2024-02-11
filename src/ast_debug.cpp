#include "../include/ast.h"

#include <string>
#include <assert.h>

static const char *binary_op_name[] = {
    "||", "&&", "==", "!=", "<", ">", "<=", ">=", 
    "+", "-", "*", "/", "%",
};

static const char *unary_op_name[] = { "-", "+", "!" };


std::string ast::BinaryExpr::debug() const {
    return '(' + lv->debug() + ") " 
        + binary_op_name[op] 
        + " (" + rv->debug() + ')'
        +'{' + std::to_string(val) + '}';
}

std::string ast::UnaryExpr::debug() const {
    return std::string(unary_op_name[op]) 
        + '(' + lv->debug() + ')'
        +'{' + std::to_string(val) + '}';
}

std::string ast::Number::debug() const {
    return std::to_string(val);
}

std::string ast::Return::debug() const {
    return "Return { " + ret_val->debug() + " }";
}

std::string ast::Block::debug() const {
    return "\tBlock {\n\t\t" + stmt->debug() + "\n\t}";
}

std::string ast::Int::debug() const {
    return "Int";
}

std::string ast::FuncDef::debug() const {
    return "FuncDef { " + func_type->debug() + ' ' + *id + "\n" + block->debug()+ "\n}";
}

std::string ast::CompUnit::debug() const {
    return func_def->debug();
}
