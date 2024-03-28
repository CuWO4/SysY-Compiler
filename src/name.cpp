#include "../include/koopa.h"
#include "../include/name.h"
#include "../include/def.h"

std::string *new_block_name() {
    return new std::string("%LLB_" + std::to_string(block_count++));
}

std::string *new_id_name() {
    return new std::string('%' + std::to_string(tmp_var_count++));
}

std::string align(std::string str, int n) {
    int blank_count = n - str.length();

    if (blank_count > 0) {
        for (int i = 0; i < blank_count; i++) str += ' ';
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
    auto res = '\t' + align(op_code + (is_i_type_inst ? "i": ""));
    if (r1 != "") res += r1;
    if (r2 != "") res += ", " + r2;
    if (r3 != "") res += ", " + r3;
    res += '\n';
    return res;
}

std::string build_comment(const koopa::Base *obj) {
    return debug_mode_riscv ? "\t# " + obj->to_string() + '\n': "";
}