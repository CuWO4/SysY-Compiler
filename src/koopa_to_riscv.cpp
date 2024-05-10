#include "koopa.h"
#include "name.h"
#include "riscv_trans.h"
#include "value_manager.h"

#include <string>

namespace koopa {

riscv_trans::Register Id::value_to_riscv(std::string& str) const {
    return riscv_trans::id_storage_map.get_storage(this)->get(str);
}

riscv_trans::Register Const::value_to_riscv(std::string& str) const {
    auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();
    str += build_inst("li", target_reg.get_lit(), std::to_string(val));
    return target_reg;
}

riscv_trans::Register MemoryDecl::rvalue_to_riscv(std::string& str) const {
    return riscv_trans::id_storage_map.get_storage(pseudo_id)->get_addr(str);
}

riscv_trans::Register Load::rvalue_to_riscv(std::string& str) const {
    auto addr_reg = riscv_trans::id_storage_map.get_storage(addr)->get(str);

    auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();

    str += build_sw_lw("lw", target_reg, 0, addr_reg);

    riscv_trans::temp_reg_manager.refresh_reg(addr_reg);

    return target_reg;
}

riscv_trans::Register GetElemPtr::rvalue_to_riscv(std::string& str) const {
    auto addr_reg = base->value_to_riscv(str);
    auto offset_reg = offset->value_to_riscv(str);
    auto size_reg = riscv_trans::temp_reg_manager.get_unused_reg();

    str += build_inst(
        "li", size_reg.get_lit(), 
        std::to_string(base->get_type()->unwrap()->unwrap()->get_byte_size())
    );
    str += build_inst("mul", offset_reg.get_lit(), offset_reg.get_lit(), size_reg.get_lit());
    str += build_inst("add", addr_reg.get_lit(), addr_reg.get_lit(), offset_reg.get_lit());

    riscv_trans::temp_reg_manager.refresh_reg(size_reg);
    riscv_trans::temp_reg_manager.refresh_reg(offset_reg);
    return addr_reg;
}

riscv_trans::Register GetPtr::rvalue_to_riscv(std::string& str) const {
    auto addr_reg = base->value_to_riscv(str);
    auto offset_reg = offset->value_to_riscv(str);
    auto size_reg = riscv_trans::temp_reg_manager.get_unused_reg();

    str += build_inst(
        "li", size_reg.get_lit(), 
        std::to_string(base->get_type()->unwrap()->get_byte_size())
    );
    str += build_inst("mul", offset_reg.get_lit(), offset_reg.get_lit(), size_reg.get_lit());
    str += build_inst("add", addr_reg.get_lit(), addr_reg.get_lit(), offset_reg.get_lit());

    riscv_trans::temp_reg_manager.refresh_reg(size_reg);
    riscv_trans::temp_reg_manager.refresh_reg(offset_reg);
    return addr_reg;
}

static riscv_trans::Register expr_inst_builder(
    std::string inst, 
    riscv_trans::Register first_reg, riscv_trans::Register second_reg, 
    std::string& str
) {
    auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();
    str += build_inst(inst, target_reg.get_lit(), first_reg.get_lit(), second_reg.get_lit());
    riscv_trans::temp_reg_manager.refresh_reg(first_reg);
    riscv_trans::temp_reg_manager.refresh_reg(second_reg);
    return target_reg;
}

static riscv_trans::Register expr_inst_builder(
    std::string inst, 
    riscv_trans::Register first_reg, 
    std::string& str
) {
    auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();
    str += build_inst(inst, target_reg.get_lit(), first_reg.get_lit());
    riscv_trans::temp_reg_manager.refresh_reg(first_reg);
    return target_reg;
}

riscv_trans::Register Eq::rvalue_to_riscv(std::string& str) const {
    if (lv->is_const()) {
        return Eq(rv, lv).rvalue_to_riscv(str);
    }

    if (rv->is_const()) {
        auto lv_reg = lv->value_to_riscv(str);
        auto tmp_reg = riscv_trans::temp_reg_manager.get_unused_reg();

        str += build_i_type_inst("xor", tmp_reg, lv_reg, rv->get_val());

        riscv_trans::temp_reg_manager.refresh_reg(lv_reg);

        return expr_inst_builder("seqz", tmp_reg, str);
    }

    auto tmp_reg = expr_inst_builder("xor", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
    return expr_inst_builder("seqz", tmp_reg, str);
}

riscv_trans::Register Ne::rvalue_to_riscv(std::string& str) const {
    if (lv->is_const()) {
        return Ne(rv, lv).rvalue_to_riscv(str);
    }

    if (rv->is_const()) {
        auto lv_reg = lv->value_to_riscv(str);
        auto tmp_reg = riscv_trans::temp_reg_manager.get_unused_reg();

        str += build_i_type_inst("xor", tmp_reg, lv_reg, rv->get_val());
        
        riscv_trans::temp_reg_manager.refresh_reg(lv_reg);

        return expr_inst_builder("snez", tmp_reg, str);
    }

    auto tmp_reg = expr_inst_builder("xor", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
    return expr_inst_builder("snez", tmp_reg, str);
}

riscv_trans::Register Gt::rvalue_to_riscv(std::string& str) const {
    return expr_inst_builder("sgt", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}

riscv_trans::Register Lt::rvalue_to_riscv(std::string& str) const {
    return expr_inst_builder("slt", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}

riscv_trans::Register Ge::rvalue_to_riscv(std::string& str) const {
    auto tmp_reg = expr_inst_builder("slt", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
    
    auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();
    
    str += build_i_type_inst("xor", target_reg, tmp_reg, 1);

    riscv_trans::temp_reg_manager.refresh_reg(tmp_reg);

    return target_reg;
}

riscv_trans::Register Le::rvalue_to_riscv(std::string& str) const {
    auto tmp_reg = expr_inst_builder("sgt", lv->value_to_riscv(str), rv->value_to_riscv(str), str);

    auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();
    
    str += build_i_type_inst("xor", target_reg, tmp_reg, 1);
    
    riscv_trans::temp_reg_manager.refresh_reg(tmp_reg);

    return target_reg;
}

riscv_trans::Register Add::rvalue_to_riscv(std::string& str) const {
    if (lv->is_const()) {
        return Add(rv, lv).rvalue_to_riscv(str);
    }

    if (rv->is_const()) {
        auto lv_reg = lv->value_to_riscv(str);

        auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();
        
        str += build_i_type_inst("add", target_reg, lv_reg, rv->get_val());

        riscv_trans::temp_reg_manager.refresh_reg(lv_reg);

        return target_reg;
    }

    return expr_inst_builder("add", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}

riscv_trans::Register Sub::rvalue_to_riscv(std::string& str) const {
    return expr_inst_builder("sub", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}

riscv_trans::Register Mul::rvalue_to_riscv(std::string& str) const {
    return expr_inst_builder("mul", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}

riscv_trans::Register Div::rvalue_to_riscv(std::string& str) const {
    return expr_inst_builder("div", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}

riscv_trans::Register Mod::rvalue_to_riscv(std::string& str) const {
    return expr_inst_builder("rem", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}

riscv_trans::Register And::rvalue_to_riscv(std::string& str) const {
    if (lv->is_const()) {
        return And(rv, lv).rvalue_to_riscv(str);
    }

    if (rv->is_const()) {
        auto lv_reg = lv->value_to_riscv(str);

        auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();
        
        str += build_i_type_inst("and", target_reg, lv_reg, rv->get_val());

        riscv_trans::temp_reg_manager.refresh_reg(lv_reg);

        return target_reg;
    }

    return expr_inst_builder("and", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}

riscv_trans::Register Or::rvalue_to_riscv(std::string& str) const {
    if (lv->is_const()) {
        return Or(rv, lv).rvalue_to_riscv(str);
    }

    if (rv->is_const()) {
        auto lv_reg = lv->value_to_riscv(str);

        auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();
        
        str += build_i_type_inst("or", target_reg, lv_reg, rv->get_val());

        riscv_trans::temp_reg_manager.refresh_reg(lv_reg);

        return target_reg;
    }

    return expr_inst_builder("or", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}

riscv_trans::Register Xor::rvalue_to_riscv(std::string& str) const {
    if (lv->is_const()) {
        return Xor(rv, lv).rvalue_to_riscv(str);
    }

    if (rv->is_const()) {
        auto lv_reg = lv->value_to_riscv(str);

        auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();
        
        str += build_i_type_inst("xor", target_reg, lv_reg, rv->get_val());

        riscv_trans::temp_reg_manager.refresh_reg(lv_reg);

        return target_reg;
    }

    return expr_inst_builder("xor", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}


riscv_trans::Register Shl::rvalue_to_riscv(std::string& str) const {
    return expr_inst_builder("sll", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}

riscv_trans::Register Shr::rvalue_to_riscv(std::string& str) const {
    return expr_inst_builder("srl", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}

riscv_trans::Register Sar::rvalue_to_riscv(std::string& str) const {
    return expr_inst_builder("sra", lv->value_to_riscv(str), rv->value_to_riscv(str), str);
}

void StoreValue::stmt_to_riscv(std::string& str, riscv_trans::TransMode trans_mode) const {
    str += build_comment(this);

    auto addr_reg = riscv_trans::id_storage_map.get_storage(addr)->get(str);

    auto val_reg = value->value_to_riscv(str);

    str += build_sw_lw("sw", val_reg, 0, addr_reg);

    riscv_trans::temp_reg_manager.refresh_reg(val_reg);
    riscv_trans::temp_reg_manager.refresh_reg(addr_reg);
}

void StoreInitializer::stmt_to_riscv(std::string& str, riscv_trans::TransMode trans_mode) const {
    str += build_comment(this);

    auto flat_vec = initializer->to_flat_vec(addr->get_type()->unwrap()->get_byte_size());

    auto addr_reg = riscv_trans::id_storage_map.get_storage(addr)->get(str);
    auto tmp_reg = riscv_trans::temp_reg_manager.get_unused_reg();

    int offset = 0;
    for (auto item: flat_vec) {
        str += build_inst("li", tmp_reg.get_lit(), std::to_string(item));
        str += build_sw_lw("sw", tmp_reg, offset, addr_reg);
        offset += 4;
    }

    riscv_trans::temp_reg_manager.refresh_reg(tmp_reg);
    riscv_trans::temp_reg_manager.refresh_reg(addr_reg);
}

void SymbolDef::stmt_to_riscv(std::string& str, riscv_trans::TransMode trans_mode) const {

    str += build_comment(this);

    auto source_reg = val->rvalue_to_riscv(str);

    riscv_trans::id_storage_map.get_storage(id)->save(str, source_reg);

    riscv_trans::temp_reg_manager.refresh_reg(source_reg);
}

void Return::stmt_to_riscv(std::string& str, riscv_trans::TransMode trans_mode) const {

    str += build_comment(this);

    if (return_type == HasRetVal) {
        auto ret_val_reg = val->value_to_riscv(str);
        str += build_inst("mv", "a0", ret_val_reg.get_lit());
        riscv_trans::temp_reg_manager.refresh_reg(ret_val_reg);
    }

    if (riscv_trans::current_has_called_func) {
        str += build_sw_lw(
            "lw", riscv_trans::Register("ra"), 
            riscv_trans::current_stack_frame_size - 4
        );
    }

    if (riscv_trans::current_stack_frame_size != 0) {
        str += build_i_type_inst(
            "add", 
            riscv_trans::Register("sp"), 
            riscv_trans::Register("sp"), 
            riscv_trans::current_stack_frame_size
        );
    }

    str += build_inst("ret");
}

void Branch::stmt_to_riscv(std::string& str, riscv_trans::TransMode trans_mode) const {
    auto cond_reg = cond->value_to_riscv(str);
    str += build_inst("bnez", cond_reg.get_lit(), to_riscv_style(target1.get_name()));
    str += build_inst("j", to_riscv_style(target2.get_name()));

    riscv_trans::temp_reg_manager.refresh_reg(cond_reg);
}

void Jump::stmt_to_riscv(std::string& str, riscv_trans::TransMode trans_mode) const {
    str += build_inst("j", to_riscv_style(target.get_name()));
}

static void func_call_to_riscv_impl(const koopa::FuncCall* self, std::string& str) {
    // no need to save registers since all identifiers so far have been stored on the stack frame

    auto self_args = self->get_args();
    for (int i = 0; i < self_args.size(); i++) {
        auto arg_reg = self_args[i]->value_to_riscv(str);

        if (i < 8) {
            str += build_inst(
                "mv",
                'a' + std::to_string(i),
                arg_reg.get_lit()
            );
        }
        else {
            str += build_sw_lw("sw", arg_reg, 4 * (i - 8));
        }

        riscv_trans::temp_reg_manager.refresh_reg(arg_reg);
    }

    str += build_inst(
        "call", to_riscv_style(self->get_id()->get_lit())
    );
}

riscv_trans::Register FuncCall::rvalue_to_riscv(std::string& str) const {
    func_call_to_riscv_impl(this, str);

    return riscv_trans::Register("a0");
}

void FuncCall::stmt_to_riscv(std::string& str, riscv_trans::TransMode trans_mode) const {
    str += build_comment(static_cast<const koopa::Stmt*>(this));

    func_call_to_riscv_impl(this, str);
}

void Block::block_to_riscv(std::string& str) const {
    str += to_riscv_style(label.get_name()) + ":\n";
    for(auto* stmt: stmts) {
        stmt->stmt_to_riscv(str, riscv_trans::trans_mode::TextSegment);
    }
}

/*
 * register function parameters into `riscv_trans::id_storage_map`
 * 
 * the first eight parameters are placed in a0-a7 in sequence, and 
 * the remaining parameters are placed on the stack frame, in order 
 * 0(sp), 4(sp), 8(sp)...
 */
void FuncDef::stmt_to_riscv(std::string& str, riscv_trans::TransMode trans_mode) const {
    if (trans_mode == riscv_trans::trans_mode::TextSegment) {

        str += build_comment(this);
        
        value_manager.enter_func(id->get_lit());

        riscv_trans::allocate_ids_storage_location(this);

        str += to_riscv_style(id->get_lit()) + ":\n";

        if (riscv_trans::current_stack_frame_size != 0) {
            str += build_i_type_inst(
                "add", 
                riscv_trans::Register("sp"), 
                riscv_trans::Register("sp"), 
                -riscv_trans::current_stack_frame_size
            );
        }

        if (riscv_trans::current_has_called_func) {
            str += build_sw_lw(
                "sw", riscv_trans::Register("ra"), 
                riscv_trans::current_stack_frame_size - 4
            );
        }

        for (auto* block: blocks) {
            block->block_to_riscv(str);
        }

        value_manager.leave_func();
    }
}

void GlobalSymbolDef::stmt_to_riscv(std::string& str, riscv_trans::TransMode trans_mode) const {
    if (trans_mode == riscv_trans::trans_mode::DataSegment) {
        riscv_trans::id_storage_map.register_id(
            id, 
            new riscv_trans::DataSeg(to_riscv_style(id->get_lit()))
        );

        str += build_comment(this);

        str += "\t.global " + to_riscv_style(id->get_lit()) + '\n';
        str += to_riscv_style(id->get_lit()) + ":\n";
        decl->stmt_to_riscv(str, trans_mode);
    }
}

void GlobalMemoryDecl::stmt_to_riscv(std::string& str, riscv_trans::TransMode trans_mode) const {
    auto flat_vec = initializer->to_flat_vec(type->get_byte_size());

    int zero_count = 0;
    for (auto item: flat_vec) {
        if (item == 0) { 
            zero_count++;
            continue; 
        }

        if (zero_count > 0) {
            str += "\t.zero " + std::to_string(zero_count * 4) + "\n";
        }

        str += "\t.word " + std::to_string(item) + "\n";
    }

    if (zero_count > 0) {
        str += "\t.zero " + std::to_string(zero_count * 4) + "\n";
    }
}

void FuncDecl::stmt_to_riscv(std::string& str, riscv_trans::TransMode trans_mode) const {
}

void Program::prog_to_riscv(std::string& str) const {
    str += "\t.data\n";
    for (auto* global_stmt: global_stmts) {
        global_stmt->stmt_to_riscv(str, riscv_trans::trans_mode::DataSegment);
    }


    str += "\t.text\n";
    str += "\t.global main\n";
    for (auto* global_stmt: global_stmts) {
        global_stmt->stmt_to_riscv(str, riscv_trans::trans_mode::TextSegment);
    }

    
}

}