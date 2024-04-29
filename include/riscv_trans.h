#ifndef RISCV_TRANS_H_
#define RISCV_TRANS_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

namespace koopa {
    class Stmt;
    class Value;
    class Block;
    class Id;
    class Label;
    class GlobalStmt;
    class FuncDef;
}

namespace riscv_trans {

    class Register;
    /*
     * storage location in riscv of koopa identifier,
     * at register, data segment or memory.
     */
    class RiscvStorage {
    public:

        /**
         * @return   get or save the value of `source_reg` to Storage Location 
         * @example  `mv    a0, t0`
         *           `lui   a0, %hi(num)
         *            li    a1, 10
         *            sw    a1, %lo(num)(a0)`
         * ! Although this breaks the design pattern, it is broken because of the
         * ! asymmetry between storing a value to a variable in a code segment and
         * ! the other two forms (requiring loading the high address in front,
         * ! freeing the save address register in the back, etc.) It is worth it,
         * ! otherwise it may require a lot of effort to reconstruct the system.
         */
        virtual std::string get(Register target_reg) = 0;
        virtual std::string save(Register source_reg) = 0;

        /**
         * @return   the literal of the value
         * @example  `t1`, `a0`, `var`, `12(sp)`
         */
        virtual std::string get_lit() = 0;
    };

    /*
     * register
     *
     *          |  ABI name |  description                      |  saver
     *  x0      | zero      | always zero                       | N/A
     *  x1      | ra        | return address                    | caller
     *  x2      | sp        | stack pointer                     | callee
     *  x3      | gp        | global pointer                    | N/A
     *  x4      | tp        | thread pointer                    | N/A
     *  x5      | t0        | temporary/alternate link register |   ...r
     *  x6-7    | t1-2      | temporary register                |   ...r
     *  x8      | s0/fp     | saving register/frame pointer     |   ...e
     *  x9      | s1        | saving register                   |   ...e
     *  x10-11  | a0-1      | function parameters/return value  |   ...r
     *  x12-17  | a2-7      | function parameters               |   ...r
     *  x18-27  | s2-11     | saving register                   |   ...e
     *  x28-31  | t3-6      | temporary register                |   ...r
     *
     * save s0 and fp at different numbers (although in the standard they are the 
     * same register and have the same number)
     */
    constexpr int REG_COUNT = 33;
    extern const char* abi_name[REG_COUNT];
    class Register: public RiscvStorage {
    public:
        Register();
        Register(int serial_num);
        /**
         * parse the lit
         * 
         * @throw    <std::string> if failed
         * @example  Register("t0")
         */
        Register(std::string lit);
        
        std::string get(Register target_reg) override;
        std::string save(Register source_reg) override;
        std::string get_lit() override;

        int get_serial_num();

    private:
        int serial_num;
    };

    // data segment
    class DataSeg: public RiscvStorage {
    public:
        DataSeg();
        DataSeg(std::string lit);

        std::string get(Register target_reg) override;
        std::string save(Register source_reg) override;
        std::string get_lit() override;

    private:
        std::string lit;
    };

    // memory
    class StackFrame: public RiscvStorage {
    public:
        StackFrame();
        StackFrame(int offset);

        std::string get(Register target_reg) override;
        std::string save(Register source_reg) override;
        std::string get_lit() override;

        int get_offset();

    private:
        int offset;
    };

    class TempRegManager {
    public:
        TempRegManager();

        Register get_unused_reg();
        /**
         * refresh register if `reg` is a temporary register, while
         * do nothing if it's not.
         */
        void refresh_reg(Register reg);

    private:
        static constexpr int TEMP_REG_COUNT = 7;
        bool is_used[TEMP_REG_COUNT];
    };
    extern TempRegManager temp_reg_manager;

    // save the storage locations assigned to identifiers in koopa for riscv
    class IdStorageMap {
    public:
        IdStorageMap();
        bool does_id_exist(const koopa::Id* id);
        /**
         * @throw  <const char*> if id is not registered
         */
        RiscvStorage* get_storage(const koopa::Id* id);
        void register_id(const koopa::Id* id, RiscvStorage* storage);

    private:
        std::unordered_map<const koopa::Id *, RiscvStorage*> map;
    };
    
    extern IdStorageMap id_storage_map;

    /*
     * stack frame size (bit) of function currently at 
     */
    extern int current_stack_frame_size;
    /*
     * whether function currently at calls function
     * if it's true, save ra at the stack frame
     */
    extern bool current_has_called_func;

    /*
     * allocate storage location for identifier & formal parameters
     * of `func_def`
     *
     * implemented in `allocate.cpp`, an interface reserved for future 
     * register allocation algorithms
     */
    void allocate_ids_storage_location(const koopa::FuncDef* func_def);

    /*
     * indicate which stage the riscv translation is currently in
     */
    namespace trans_mode {
        enum TransMode {
            DataSegment,
            TextSegment
        };
    }
    using TransMode = trans_mode::TransMode;
}


#endif