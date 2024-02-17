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
        + " (" + rv->debug() + ')';
}

std::string ast::UnaryExpr::debug() const {
    return std::string(unary_op_name[op]) 
        + '(' + lv->debug() + ')';
}

std::string ast::Id::debug() const {
    return *lit;
}

std::string ast::Number::debug() const {
    return std::to_string(val);
}

std::string ast::VarDef::debug() const {
    return id->debug() +
        (has_init ? " = " + init->debug() : "") + ',';
}

std::string ast::VarDecl::debug() const {
    auto res = type->debug() + ' ';

    for (auto var_def : var_defs) {
        res += var_def->debug();
    }

    return res;
}

std::string ast::Assign::debug() const {
    return *id->lit + " = " + rval->debug();
}

std::string ast::Return::debug() const {
    return "Return { " + ret_val->debug() + " }";
}

std::string ast::Block::debug() const {
    auto res = std::string("");

    res += "\tBlock {\n";

    for (auto stmt : stmts) {
        res += "\t\t" + stmt->debug() + '\n';
    }

    res += "\t}";

    return res;
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
