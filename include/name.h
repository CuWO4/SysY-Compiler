#ifndef NAME_H_
#define NAME_H_

#include "koopa.h"
#include "def.h"

#include <string>

/**
 * @example  `%LLB_1` `%4`
 */
std::string new_block_name();
std::string new_id_name();

/*
 * align `str` to be at least `n` characters long
 */
std::string align(std::string str, int n = 8);

/**
 * convert `symbol` to riscv-style
 * @example  `@var` -> `var` 
 */
std::string to_riscv_style(std::string symbol);

/**
 * add `\t` at the beginning, assign `op_code` to 8 characters long,
 * add comma between each two operands, add space after comma.
 * 
 * @param is_i_type_inst  turn `op_code` to i-type instruction
 * @example  build_inst("add", "a0", "t1", "1", true)
 *              -> `    addi    a0, t1, 1` 
 */
std::string build_inst(std::string op_code, 
    std::string r1 = {}, std::string r2 = {}, std::string r3 = {}, 
    bool is_i_type_inst = false
);

/**
 * @example  build_mem(4, Register("sp")) => "4(sp)"
 */
std::string build_mem(
    int offset, 
    riscv_trans::Register base_addr = riscv_trans::Register("sp")
);

/**
 * @return  string form of the source koopa line if in `debug_mode_riscv` is true
 */
std::string build_comment(const koopa::Base* obj);

/**
 * handling `offset` beyond the IMM12 range
 * @example  build_sw_lw(lw, t0, 4) =>  
 *      lw      t0, 4(sp)
 * @example  build_sw_lw(lw, t0, 10000) =>
 *      li      t1, 10000
 *      add     t1, t1, sp
 *      lw      t0, 0(t1)   
 */
std::string build_sw_lw(
    std::string inst,
    riscv_trans::Register val_reg, 
    int offset, 
    riscv_trans::Register addr_reg = riscv_trans::Register("sp")
);

std::string build_i_type_inst(
    std::string inst, 
    riscv_trans::Register target_reg,
    riscv_trans::Register first_reg, 
    int second_val
);

#endif