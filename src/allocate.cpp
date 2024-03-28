#include "../include/riscv_trans.h"

#include "../include/value_manager.h"

void riscv_trans::allocate_ids_storage_location(std::string func_id_lit) {
    /*
     * naive strategy: allocate all identifiers to stack frame
     */

    auto ids = value_manager.get_func_ids(func_id_lit);

    int stack_frame_size = 4 * ids.size();

    stack_frame_size = (stack_frame_size / 16 + 1) * 16; // align to 4bit

    current_stack_frame_size = stack_frame_size;

    for (auto id : ids) {
        stack_frame_size -= 4;
        riscv_trans::id_storage_map.register_id(id, new riscv_trans::Memory(stack_frame_size));
    }
}