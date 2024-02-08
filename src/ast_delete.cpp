#include "../include/ast.h"

ast::Block::~Block() {
    delete stmt;
}

ast::FuncDef::~FuncDef() {
    delete func_type;
    delete id;
    delete block;
}

ast::CompUnit::~CompUnit() {
    delete func_def;
}