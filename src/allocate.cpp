#include <typeinfo>

#include "../include/riscv_trans.h"

#include "../include/value_manager.h"

static int max(int a, int b) { return a > b ? a : b; }

/**
 * @return the maximum number of parameters of the functions being called 
 * @example  max_called_func_param_n( main() { f(1, 2); g(1, 2, 3, 4); } ) => 4
 */
static int get_max_called_func_param_n(const koopa::FuncDef* func_def) {
    int result = 0;
    for (auto block: func_def->blocks) {
        for (auto stmt: block->stmts) {
            // ! ugly, though works
            if (typeid(*stmt) == typeid(koopa::FuncCall)) {
                result = max(
                    result, 
                    static_cast<koopa::FuncCall*>(stmt)->args.size()
                );
            }
            else if (
                typeid(*stmt) == typeid(koopa::SymbolDef) 
                && typeid(*static_cast<koopa::SymbolDef*>(stmt)->val) 
                    == typeid(koopa::FuncCall)
            ) {
                result = max(
                    result, 
                    static_cast<koopa::FuncCall*>(
                        static_cast<koopa::SymbolDef*>(stmt)->val
                    )->args.size()
                );
            }

        }
    }
    return result;
}

/**
 * @return whether function is called in the body of func_def
 */
static bool has_called_func(const koopa::FuncDef* func_def) {
    for (auto block: func_def->blocks) {
        for (auto stmt: block->stmts) {
            if (
                typeid(*stmt) == typeid(koopa::FuncCall)
                || (
                    typeid(*stmt) == typeid(koopa::SymbolDef) 
                    && typeid(*static_cast<koopa::SymbolDef*>(stmt)->val) 
                        == typeid(koopa::FuncCall)
                )
            ) {
                return true;
            }

        }
    }
    return false;
}

static void allocate_location(const koopa::FuncDef* func_def, int stack_frame_size) {
    if (riscv_trans::current_has_called_func) {
        stack_frame_size -= 4; // reserve for ra
    }

    for (auto id : value_manager.get_func_ids(func_def->id->lit)) {
        stack_frame_size -= 4;
        riscv_trans::id_storage_map.register_id(
            id, 
            new riscv_trans::StackFrame(stack_frame_size)
        );
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
                new riscv_trans::StackFrame(
                    riscv_trans::current_stack_frame_size + 4 * (param_count - 8)
                )
            );
        }
        param_count++;
    }
}

static int get_stack_frame_size(const koopa::FuncDef* func_def) {
    int stack_frame_size = 4 * value_manager.get_func_ids(func_def->id->lit).size();

    /*
     * to save ra
     */
    if (riscv_trans::current_has_called_func) {
        stack_frame_size += 4;
    }

    int max_called_func_param_n = get_max_called_func_param_n(func_def);
    /*
     * to save callee's arguments
     */
    if (max_called_func_param_n > 8) {
        stack_frame_size += 4 * (max_called_func_param_n - 8);
    }

    stack_frame_size = (stack_frame_size / 16 + 1) * 16; // align to 4bit

    return stack_frame_size;
}

void riscv_trans::allocate_ids_storage_location(const koopa::FuncDef* func_def) {
    /*
     * naive strategy: allocate all identifiers to stack frame
     */

    current_has_called_func = has_called_func(func_def);

    current_stack_frame_size = get_stack_frame_size(func_def);
    
    allocate_location(func_def, current_stack_frame_size);
}