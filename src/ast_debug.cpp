#include "../include/ast.h"

#include <iostream>

void ReturnStmtAst::debug() const {
    std::cout << "ReturnStmt { " << return_val << " }, ";
}

void BlockAst::debug() const {
    std::cout << "Block { ";
    stmt->debug();
    std::cout << "}, ";
}

void IntFuncTypeAst::debug() const {
    std::cout << "IntFuncType, ";
}

void FuncDefAst::debug() const {
    std::cout << "FuncDef { ";
    func_type->debug();
    std::cout << *id << " , ";
    block->debug();
    std::cout << "}, ";
}

void CompUnitAst::debug() const {
    std::cout << "CompUnit { ";
    func_def->debug();
    std::cout << "} ";
}
