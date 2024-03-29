#include "../include/riscv_trans.h"

#include "../include/name.h"

namespace riscv_trans {

    const char *abi_name[33] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "fp",
        "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", 
        "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
        "t3", "t4", "t5", "t6"
    };

    TempRegManager temp_reg_manager;

    IdStorageMap id_storage_map;

    int current_stack_frame_size = 0;
    bool current_has_called_func = false;

    Register::Register(): serial_num(0) {}
    Register::Register(int serial_num): serial_num(serial_num) {}
    Register::Register(std::string lit) {
        for (int i = 0; i < REG_COUNT; i++) {
            if (abi_name[i] == lit) {
                serial_num = i;
                return;
            }
        }

        throw "unknown register `" + lit + '`';
    }

    std::string Register::get(Register target_reg) { 
        return build_inst(
            "mv",
            target_reg.get_lit(),
            get_lit()
        ); 
    }
    std::string Register::save(Register source_reg) {
        return build_inst(
            "mv",
            get_lit(),
            source_reg.get_lit()
        );
    }
    std::string Register::get_lit() { return abi_name[serial_num]; }
    int Register::get_serial_num() { return serial_num; }


    DataSeg::DataSeg() : lit("") {}
    DataSeg::DataSeg(std::string lit): lit(lit) {}

    std::string DataSeg::get(Register target_reg) { 
        return build_inst(
            "la",
            target_reg.get_lit(),
            get_lit()
        ); 
    }
    std::string DataSeg::save(Register source_reg) {
        // TODO
        return "";
    }
    std::string DataSeg::get_lit() { return lit; }


    Memory::Memory(): offset(0) {}
    Memory::Memory(int offset): offset(offset) {}

    std::string Memory::get(Register target_reg) { 
        return build_inst(
            "lw",
            target_reg.get_lit(),
            get_lit()
        ); 
    }
    std::string Memory::save(Register source_reg) {
        return build_inst(
            "sw",
            source_reg.get_lit(),
            get_lit()
        );
    }
    std::string Memory::get_lit() { return std::to_string(offset) + "(sp)"; }


    TempRegManager::TempRegManager() {
        for (int i = 0; i < TEMP_REG_COUNT; i++) {
            is_used[i] = false;
        }
    }

    Register TempRegManager::get_unused_reg() {
        for (int i = 0; i < TEMP_REG_COUNT; i++) {
            if (!is_used[i]) {
                is_used[i] = true;
                return Register("t" + std::to_string(i));
            }
        }

        throw "register exhausted";
    }

    void TempRegManager::refresh_reg(Register reg) {
        auto lit = reg.get_lit();

        if (lit.at(0) != 't') return;

        is_used[lit.at(1) - '0'] = false;
    }

    IdStorageMap::IdStorageMap(): map({}) {}

    bool IdStorageMap::does_id_exist(const koopa::Id *id) {
        return map.find(id) != map.end();
    }

    RiscvStorage *IdStorageMap::get_storage(const koopa::Id *id) {
        auto res = map.find(id);

        if (res == map.end()) {
            throw "identifier `" + *id->lit + "` is not registered in `IdStorageMap`";
        }

        return res->second;
    }

    void IdStorageMap::register_id(const koopa::Id *id, RiscvStorage *storage) {
        map.insert(
            std::pair<const koopa::Id *, RiscvStorage *>(id, storage)
        );
    }

}