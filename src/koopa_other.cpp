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