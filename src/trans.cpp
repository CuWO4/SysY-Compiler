#include "../include/trans.h"
#include "../include/koopa.h"
#include "../include/def.h"

namespace koopa_trans {

std::vector<koopa::Block *> Blocks::to_raw_blocks() {
    auto res = std::vector<koopa::Block *>{};
    res.reserve(1 + blocks.size());

    res.push_back( 
        new koopa::Block(
            new koopa::Id(new koopa::Label, new std::string('%' + std::to_string(block_count++))),
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