#include "../include/ast.h"

#include <iostream>

void ast::Return::debug() const {
    std::cout << "Return { " << return_val << " }, ";
}

void ast::Block::debug() const {
    std::cout << "Block { ";
    stmt->debug();
    std::cout << "}, ";
}

void ast::IntFuncType::debug() const {
    std::cout << "IntFuncType, ";
}

void ast::FuncDef::debug() const {
    std::cout << "FuncDef { ";
    func_type->debug();
    std::cout << *id << " , ";
    block->debug();
    std::cout << "}, ";
}

void ast::CompUnit::debug() const {
    std::cout << "CompUnit { ";
    func_def->debug();
    std::cout << "} ";
}
