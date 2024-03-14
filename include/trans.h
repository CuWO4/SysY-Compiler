#ifndef TRANS_H_
#define TRANS_H_

#include <vector>
#include <string>

namespace koopa {
    class Stmt;
    class Value;
    class Block;
    class Id;
    class Label;
    class GlobalStmt;
}

namespace riscv_trans {
    class Info {
    public:
        int stack_frame_size = 0;

        std::string res_lit = "";

        static constexpr int REG_COUNT = 7;
        bool is_reg_used[REG_COUNT] = { 0 };
        std::string get_unused_reg();

        void refresh_reg(std::string lit);
    };

    enum TransMode {
        DataSegment,
        TextSegment
    };
}

namespace koopa_trans {
    /**
     *
     *  serve as koopa translation intermediate variable
     *
     *  structure:
     *
     *          stmt        ----+
     *          stmt            |====> active_stmts
     *          ...             |       * do not belong to any block
     *          stmt        ----+
     *                                  * will be merged into the active block when merging two `Blocks` or converted
     *      block           ----+         into a block by calling `to_raw_blocks`
     *          stmt            |
     *          stmt            |====> blocks
     *          ...             |       * may be empty
     *          stmt            |
     *                          |       * the last block is active if one exists; otherwise, `active_stmts` would be treated
     *      block               |         as an active block
     *          stmt            |
     *          stmt            |       * when merging two `Blocks`, the `active_stmts` of the latter one will be merged into the
     *          ...             |         active block of the first, and then `blocks` will be appended to the first
     *          stmt            |
     *      ...             ----+
     *
     *      when `blocks` is empty, `last_val` saves the pointer of last value active_stmts defines, for instance in 
     *          %1 = add %0, 1
     *          %2 = mul %1, 2
     *      , `last_val` = %2, callee may capture and use it in following compiling. when merging two `Blocks`, the last_val 
     *      of new `Blocks` will the latter one's.
     *
     *  examples:
     *
     *      stmts][block][block] + stmts][block]  =>  stmts][block][block+stmts][block]
     *                      ^               ^                                      ^
     *                    active          active                                 active
     *
     *      stmts1] + stmts2][block]  =>  stmts1+stmts2][block]
     *        ^                 ^                          ^
     *      active            active                     active
     *
     *      stmts][block][block].to_raw_blocks  =>  [stmts][block][block]
     *
     */
    class Blocks {
    public:
        Blocks(std::vector<koopa::Stmt *> stmts = {});
        Blocks(std::vector<koopa::Stmt *> stmts, koopa::Value *last_val);

        koopa::Id *get_begin_block_id();

        void throw_last_val();
        void set_last_val(koopa::Value *new_last_val);
        koopa::Value *get_last_val();

        std::vector<koopa::Stmt *> active_stmts = {};
        std::vector<koopa::Block *> blocks = {};

        std::vector<koopa::Block *> to_raw_blocks();

        friend void operator+=(Blocks &self, Blocks &other);
        friend void operator+=(Blocks &self, std::vector<koopa::Block *> blocks);
        friend void operator+=(Blocks &self, koopa::Block * block);
        friend void operator+=(Blocks &self, std::vector<koopa::Stmt *> &stmts);
        friend void operator+=(Blocks &self, koopa::Stmt *stmt);

        std::string to_string() const { return ""; }
        void to_riscv(std::string &str, riscv_trans::Info &info) const {}

    private:
        bool has_last_val = false;
        koopa::Value *last_val = nullptr;

        koopa::Id *begin_block_id = nullptr;

        void init_begin_block_id();
    };

    class GlobalStmts {
    public:
        GlobalStmts();
        GlobalStmts(koopa::GlobalStmt *global_stmt);
        GlobalStmts(std::vector<koopa::GlobalStmt *> global_stmts);

        std::vector<koopa::GlobalStmt *> &to_raw_vector();

        friend void operator+=(GlobalStmts &self, GlobalStmts &other);
        friend void operator+=(GlobalStmts &self, std::vector<koopa::GlobalStmt *> &global_stmts);
        friend void operator+=(GlobalStmts &self, koopa::GlobalStmt *global_stmt);

    private:
        std::vector<koopa::GlobalStmt *> global_stmts = {};
    };

}

#endif