#include "../include/koopa.h"

int koopa::Block::get_stack_frame_size() {

    int stack_frame_size = 0;

    for (auto stmt : stmts)  {
        if (!stmt->is_unit) stack_frame_size += 4;
    }

    return stack_frame_size;
}

void koopa::Block::set_id_offset(int &offset) {

    for (auto stmt : stmts) {
        if (typeid(*stmt) == typeid(koopa::SymbolDef)) {
            offset -= 4;
            static_cast<koopa::SymbolDef *>(stmt)->id->sf_offset = offset;
        }
    }

}

namespace koopa {
    namespace op {
        std::function<int(int, int)> op_func[] = {
            [] (int a, int b) { return a != b; },
            [] (int a, int b) { return a == b; },
            [] (int a, int b) { return a > b; },
            [] (int a, int b) { return a < b; },
            [] (int a, int b) { return a >= b; },
            [] (int a, int b) { return a <= b; },
            [] (int a, int b) { return a + b; },
            [] (int a, int b) { return a - b; },
            [] (int a, int b) { return a * b; },
            [] (int a, int b) { return a / b; },
            [] (int a, int b) { return a % b; },
            [] (int a, int b) { return a & b; },
            [] (int a, int b) { return a | b; },
            [] (int a, int b) { return a ^ b; },
            [] (int a, int b) { return a << b; },
            [] (int a, int b) { return (unsigned)a >> (unsigned)b; },
            [] (int a, int b) { return a >> b; },
        };
    }
}

std::string riscv_trans::Info::get_unused_reg() {
    for (int i = 0; i < 7; i++) {
        if (is_reg_used[i] == false) {
            is_reg_used[i] = true;
            return 't' + std::to_string(i);
        }
    }
    throw "not enough reg";
}

void riscv_trans::Info::refresh_reg(std::string lit) {
    int i = lit.at(1) - '0';
    assert(i >= 0 && i <= 6);
    is_reg_used[i] = false;
}