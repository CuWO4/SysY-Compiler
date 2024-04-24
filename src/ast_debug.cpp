#include "../include/ast.h"

#include <string>

namespace ast {

std::string build_indent(int indent) {
    std::string res = "";
    for (int i = 0; i < indent; i++) res += "  ";
    return res;
}

std::string LogicOr::debug(int indent) const {
    return '(' + lv->debug() + ") || (" + rv->debug() + ')';
}

std::string LogicAnd::debug(int indent) const {
    return '(' + lv->debug() + ") && (" + rv->debug() + ')';
}

std::string Eq::debug(int indent) const {
    return '(' + lv->debug() + ") == (" + rv->debug() + ')';
}

std::string Neq::debug(int indent) const {
    return '(' + lv->debug() + ") != (" + rv->debug() + ')';
}

std::string Lt::debug(int indent) const {
    return '(' + lv->debug() + ") < (" + rv->debug() + ')';
}

std::string Gt::debug(int indent) const {
    return '(' + lv->debug() + ") > (" + rv->debug() + ')';
}

std::string Leq::debug(int indent) const {
    return '(' + lv->debug() + ") <= (" + rv->debug() + ')';
}

std::string Geq::debug(int indent) const {
    return '(' + lv->debug() + ") >= (" + rv->debug() + ')';
}

std::string Add::debug(int indent) const {
    return '(' + lv->debug() + ") + (" + rv->debug() + ')';
}

std::string Sub::debug(int indent) const {
    return '(' + lv->debug() + ") - (" + rv->debug() + ')';
}

std::string Mul::debug(int indent) const {
    return '(' + lv->debug() + ") * (" + rv->debug() + ')';
}

std::string Div::debug(int indent) const {
    return '(' + lv->debug() + ") / (" + rv->debug() + ')';
}

std::string Mod::debug(int indent) const {
    return '(' + lv->debug() + ") % (" + rv->debug() + ')';
}

std::string Comma::debug(int indent) const {
    return '(' + lv->debug() + ") , (" + rv->debug() + ')';
}

std::string Assign::debug(int indent) const {
    return '(' + lv->debug() + ") = (" + rv->debug() + ')';
}

std::string Neg::debug(int indent) const {
    return "-(" + lv->debug() + ')';
}

std::string Pos::debug(int indent) const {
    return "+(" + lv->debug() + ')';
}

std::string Not::debug(int indent) const {
    return "!(" + lv->debug() + ')';
}

std::string Id::debug(int indent) const {
    return lit 
        + '(' + std::to_string(nesting_info->nesting_level) 
        + ':' + std::to_string(nesting_info->nesting_count) + ')';
}

std::string FuncCall::debug(int indent) const {
    auto res = build_indent(indent) + func_id->debug();

    res += '(';
    for (auto actual_param: actual_params) {
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
    return 
        build_indent(indent)
        + (decl_type == decl_type::ConstDecl ? "const " : "")
        + type->debug() + ' '
        + id->debug()
        + (has_init ? " = " + init->debug() : "");
}

std::string VarDecl::debug(int indent) const {
    std::string res = "";
    for (auto var_def: var_defs) {
        res += var_def->debug(indent) + '\n';
    }
    if (var_defs.size() > 0) {
        res.pop_back(); // `'\n`
    }
    return res;
}

std::string Return::debug(int indent) const {
    return build_indent(indent) + "Return " 
        + (return_type == return_type::HasRetVal ? ret_val->debug(): "");
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
    return build_indent(indent) + "While ( " 
        + cond->debug() + " ) {\n"
        + body->debug(indent + 1) + '\n'
        + build_indent(indent) + "}";
}

std::string For::debug(int indent) const {
    return  build_indent(indent) + "For ( " 
        + init_stmt->debug() + "; " 
        + cond->debug() + "; " 
        + iter_stmt->debug() + ") {\n"
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

    for (auto stmt: stmts) {
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

std::string Pointer::debug(int indent) const {
    return '(' + pointed_type->debug() + ")[]";
}

std::string Array::debug(int indent) const {
    return '(' + element_type->debug() + ")[" + length->debug() + ']';
}

std::string FuncDef::debug(int indent) const {
    auto res = build_indent(indent) + ret_type->debug() + ' ' + id->lit + '(';
    for (auto param: params) {
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

std::string GlobalVarDef::debug(int indent) const {
    return 
        build_indent(indent)
        + (decl_type == decl_type::ConstDecl ? "const " : "")
        + type->debug() + ' '
        + id->debug()
        + (has_init ? " = " + init->debug() : "");
}

std::string GlobalVarDecl::debug(int indent) const {
    std::string res = "";
    for (auto var_def: var_defs) {
        res += var_def->debug(indent) + '\n';
    }
    if (var_defs.size() > 0) {
        res.pop_back(); // `'\n`
    }
    return res;
}

std::string CompUnit::debug(int indent) const {
    std::string res = "";

    for (auto global_stmt: global_stmts) {
        res += global_stmt->debug() + '\n';
    }
    
    return res;
}

}