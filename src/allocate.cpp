#include "../include/riscv_trans.h"

#include "../include/value_manager.h"

void riscv_trans::allocate_ids_storage_location(const koopa::FuncDef *func_def) {
    /*
     * naive strategy: allocate all identifiers to stack frame
     */

    auto ids = value_manager.get_func_ids(*func_def->id->lit);

    int stack_frame_size = 4 * ids.size();

    // TODO  optimize and make nice
    current_has_called_func = false;
    for (auto block: func_def->blocks) {
        for (auto stmt: block->stmts) {
            // ! ugly, though works
            if (
                typeid(*stmt) == typeid(koopa::FuncCall)
                || (typeid(*stmt) == typeid(koopa::SymbolDef) 
                    && typeid(*static_cast<koopa::SymbolDef *>(stmt)->val) == typeid(koopa::FuncCall))
            ) {
                current_has_called_func = true;
            }
        }
    }

    /*
     * to save ra
     */
    if (current_has_called_func) {
        stack_frame_size += 4;
    }

    if (func_def->formal_param_ids.size() > 8) {
        stack_frame_size += 4 * (func_def->formal_param_ids.size() - 8);
    }

    stack_frame_size = (stack_frame_size / 16 + 1) * 16; // align to 4bit

    current_stack_frame_size = stack_frame_size;

    if (current_has_called_func) {
        stack_frame_size -= 4; // reserve for ra
    }

    for (auto id : ids) {
        stack_frame_size -= 4;
        riscv_trans::id_storage_map.register_id(id, new riscv_trans::Memory(stack_frame_size));
    }

    int param_count = 0;
    for (auto id: func_def->formal_param_ids) {
        if (param_count < 8) {
            riscv_trans::id_storage_map.register_id(
                id, 
                new riscv_trans::Register("a" + std::to_string(param_count))
            );
        }
        else {
            riscv_trans::id_storage_map.register_id(
                id, 
                new riscv_trans::Memory(4 * (param_count - 8))
            );
        }
        param_count++;
    }
}