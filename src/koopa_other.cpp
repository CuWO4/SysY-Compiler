#include "../include/koopa.h"

namespace koopa {

int Block::get_stack_frame_size() {

    int stack_frame_size = 0;

    for (auto stmt : stmts)  {
        if (!stmt->is_unit) stack_frame_size += 4;
    }

    return stack_frame_size;
}

void Block::set_id_offset(int &offset) {

    for (auto stmt : stmts) {
        if (typeid(*stmt) == typeid(SymbolDef)) {
            offset -= 4;
            static_cast<SymbolDef *>(stmt)->id->sf_offset = offset;
        }
    }

}

void operator+=(Block &self, Stmt *stmt) {
    self.stmts.push_back(stmt);
}

void operator+=(Block &self, std::vector<Stmt *> stmts) {
    self.stmts.reserve(self.stmts.size() + stmts.size());
    self.stmts.insert(self.stmts.end(), stmts.begin(), stmts.end());
}

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