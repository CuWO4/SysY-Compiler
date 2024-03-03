#include "../include/trans.h"
#include "../include/koopa.h"
#include "../include/block_name.h"

namespace koopa_trans {

std::vector<koopa::Block *> Blocks::to_raw_blocks() {
    auto res = std::vector<koopa::Block *>{};
    res.reserve(1 + blocks.size());

    res.push_back( 
        new koopa::Block(
            new koopa::Id(new koopa::Label, new_block_name()),
            active_stmts
        ) 
    );

    res.insert(res.end(), blocks.begin(), blocks.end());

    return res;
    //! memory leak
}

void operator+=(koopa_trans::Blocks &self, koopa_trans::Blocks &other) {

    self.last_val = other.last_val;

    if (self.blocks.empty()) {
        self.active_stmts.reserve(self.active_stmts.size() + other.active_stmts.size());
        self.active_stmts.insert(self.active_stmts.end(), other.active_stmts.begin(), other.active_stmts.end());

        self.blocks = other.blocks;
    }
    else {
        *self.blocks.back() += other.active_stmts;

        self.blocks.reserve(self.blocks.size() + other.blocks.size());
        self.blocks.insert(self.blocks.end(), other.blocks.begin(), other.blocks.end());
    }
}

void operator+=(Blocks &self, std::vector<koopa::Block *> blocks) {
    self.blocks.reserve(self.blocks.size() + blocks.size());
    self.blocks.insert(self.blocks.end(), blocks.begin(), blocks.end());
}

void operator+=(Blocks &self, koopa::Block * block) {
    self.blocks.push_back(block);
}

void operator+=(koopa_trans::Blocks &self, std::vector<koopa::Stmt *> &stmts) {
    if (self.blocks.empty()) {
        self.active_stmts.reserve(self.active_stmts.size() + stmts.size());
        self.active_stmts.insert(self.active_stmts.end(), stmts.begin(), stmts.end());
    }
    else {
        *self.blocks.back() += stmts;
    }
}

void operator+=(koopa_trans::Blocks &self, koopa::Stmt *stmt) {
    if (self.blocks.empty()) {
        self.active_stmts.push_back(stmt);
    }
    else {
        *self.blocks.back() += stmt;
    }
}

}

namespace riscv_trans {

std::string Info::get_unused_reg() {
    for (int i = 0; i < 7; i++) {
        if (is_reg_used[i] == false) {
            is_reg_used[i] = true;
            return 't' + std::to_string(i);
        }
    }
    throw "not enough reg";
}

void Info::refresh_reg(std::string lit) {
    int i = lit.at(1) - '0';
    assert(i >= 0 && i <= 6);
    is_reg_used[i] = false;
}

}