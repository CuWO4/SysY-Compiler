#include "../include/koopa_trans.h"
#include "../include/koopa.h"
#include "../include/name.h"
#include "../include/value_manager.h"

namespace koopa_trans {

Blocks::Blocks(std::vector<koopa::Stmt *> stmts)
    : active_stmts(stmts), blocks({}), 
    has_last_val(true), last_val(nullptr),
    begin_block_id(nullptr) {}

Blocks::Blocks(std::vector<koopa::Stmt *> stmts, koopa::Value *last_val)
    : active_stmts(stmts), blocks({}),
    has_last_val(true), last_val(last_val),
    begin_block_id(nullptr) {
    assert(last_val);
}

std::vector<koopa::Block *> Blocks::to_raw_blocks() {
    auto res = std::vector<koopa::Block *>{};
    res.reserve(1 + blocks.size());

    res.push_back( 
        new koopa::Block(
            begin_block_id != nullptr 
                ? begin_block_id
                : value_manager.new_id(
                    koopa::id_type::BlockLabel, 
                    new koopa::Label, 
                    new_block_name()
                ),
            active_stmts
        ) 
    );

    res.insert(res.end(), blocks.begin(), blocks.end());

    return res;
    //! memory leak
}

void Blocks::init_begin_block_id() {
    begin_block_id = value_manager.new_id(
        koopa::id_type::BlockLabel, 
        new koopa::Label, 
        new_block_name()
    );
}

koopa::Id *Blocks::get_begin_block_id() {
    if (begin_block_id != nullptr) return begin_block_id;

    init_begin_block_id();
    return begin_block_id;
}

void Blocks::throw_last_val() {
    has_last_val = false;
}

void Blocks::set_last_val(koopa::Value *new_last_val) {
    has_last_val = true;
    last_val = new_last_val;
}

koopa::Value *Blocks::get_last_val() {
    if (!has_last_val) {
        throw "try to get a value from an expression that returns no value";
    }
    return last_val;
}

void operator+=(koopa_trans::Blocks &self, koopa_trans::Blocks &other) {

    if (other.has_last_val) self.last_val = other.last_val;

    if (self.blocks.empty()) {
        self.active_stmts.reserve(self.active_stmts.size() + other.active_stmts.size());
        self.active_stmts.insert(
            self.active_stmts.end(), 
            other.active_stmts.begin(), 
            other.active_stmts.end()
        );

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

GlobalStmts::GlobalStmts(): global_stmts({}) {}

GlobalStmts::GlobalStmts(koopa::GlobalStmt *global_stmt) 
    : global_stmts({global_stmt}) {
    assert(global_stmt);
}

GlobalStmts::GlobalStmts(std::vector<koopa::GlobalStmt *> global_stmts) 
    : global_stmts(global_stmts){}

std::vector<koopa::GlobalStmt *> &GlobalStmts::to_raw_vector() {
    return global_stmts;
}

void operator+=(GlobalStmts &self, GlobalStmts &other) {
    self += other.global_stmts;
}
void operator+=(GlobalStmts &self, std::vector<koopa::GlobalStmt *> &global_stmts) {
    self.global_stmts.reserve(self.global_stmts.size() + global_stmts.size());
    self.global_stmts.insert(
        self.global_stmts.end(), 
        global_stmts.begin(), 
        global_stmts.end()
    );
}

void operator+=(GlobalStmts &self, koopa::GlobalStmt *global_stmt) {
    self.global_stmts.push_back(global_stmt);
}

}