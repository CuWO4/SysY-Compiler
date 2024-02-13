#include "../include/koopa.h"

#include <string>

static std::string to_riscv_style(std::string symbol) {
    return symbol.substr(1, symbol.length() - 1);
}

std::string koopa::Id::to_riscv() const {
    if ((typeid(*type) == typeid(koopa::FuncType))) {
        return to_riscv_style(*lit);
    }
    else {
        //TODO
        return "";
    }
}

std::string koopa::Const::to_riscv() const {
    return std::to_string(val);
}

std::string koopa::Expr::to_riscv() const {
    //TODO
    return "";
}

std::string koopa::SymbolDef::to_riscv() const {
    //TODO
    return "";
}

std::string koopa::Return::to_riscv() const {
    return "\tli\ta0,\t" + val->to_riscv() + '\n'
        + "\tret\n";
}

std::string koopa::Block::to_riscv() const {
    auto res = std::string("");

    for(auto stmt : stmts) {
        res += stmt->to_riscv();
    }

    return res;
}

std::string koopa::FuncDef::to_riscv() const {
    auto res = std::string("");

    res += id->to_riscv() + ":\n";

    for (auto block : blocks) {
        res += block->to_riscv();
    }

    return res;
}

std::string koopa::Program::to_riscv() const {
    auto res = std::string("");

    res += "\t.text\n";
    res += "\t.global main\n";

    for (auto global_stmt : global_stmts) {
        res += global_stmt->to_riscv();
    }

    return res;
}