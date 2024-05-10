#include "koopa.h"
#include "name.h"
#include "def.h"

#include <algorithm>

std::string new_block_name() {
    return "%LLB_" + std::to_string(block_count++);
}

std::string new_id_name() {
    return '%' + std::to_string(tmp_var_count++);
}

std::string align(std::string str, int n) {
    size_t blank_count { n - str.length() };

    if (blank_count > 0) {
        for (int i { 0 }; i < blank_count; i++) str += ' ';
    }

    return str;
}

std::string to_riscv_style(std::string symbol) {
    return symbol.substr(1);
}

std::string build_inst(
    std::string op_code, 
    std::string r1, std::string r2, std::string r3, 
    bool is_i_type_inst
) {
    auto res { '\t' + align(op_code + (is_i_type_inst ? "i": "")) };
    if (r1 != "") res += r1;
    if (r2 != "") res += ", " + r2;
    if (r3 != "") res += ", " + r3;
    res += '\n';
    return res;
}

std::string build_mem(int offset, riscv_trans::Register base_addr) {
    return std::to_string(offset) + '(' + base_addr.get_lit() + ')';
}

std::string build_comment(const koopa::Base* obj) {
    auto str { obj->to_string() };
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());

    return debug_mode_riscv ? "\t# " + str + '\n': "";
}

std::string build_sw_lw(
    std::string inst, 
    riscv_trans::Register val_reg, 
    int offset, 
    riscv_trans::Register addr_reg
) {
    if (riscv_trans::is_within_imm12_range(offset)) {
        return build_inst(inst, val_reg.get_lit(), build_mem(offset, addr_reg));
    }
    else {
        auto res { std::string() };

        auto tmp_reg { riscv_trans::temp_reg_manager.get_unused_reg() };

        res += build_inst("li", tmp_reg.get_lit(), std::to_string(offset));
        res += build_inst("add", tmp_reg.get_lit(), tmp_reg.get_lit(), addr_reg.get_lit());
        res += build_inst(inst, val_reg.get_lit(), build_mem(0, tmp_reg));

        riscv_trans::temp_reg_manager.refresh_reg(tmp_reg);

        return res;
    }
}

std::string build_i_type_inst(
    std::string inst, 
    riscv_trans::Register target_reg,
    riscv_trans::Register first_reg, 
    int second_val
) {
    if (riscv_trans::is_within_imm12_range(second_val)) {
        return build_inst(
            inst + "i", target_reg.get_lit(), 
            first_reg.get_lit(), std::to_string(second_val)
        );
    }
    else {
        auto res { std::string() };
        auto tmp_reg { riscv_trans::temp_reg_manager.get_unused_reg() };

        res += build_inst("li", tmp_reg.get_lit(), std::to_string(second_val));
        res += build_inst(inst, target_reg.get_lit(), first_reg.get_lit(), tmp_reg.get_lit());

        riscv_trans::temp_reg_manager.refresh_reg(tmp_reg);

        return res;
    }
}