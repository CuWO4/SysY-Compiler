#include "../include/koopa.h"
#include "../include/name.h"
#include "../include/riscv_trans.h"
#include "../include/value_manager.h"

#include <string>

namespace koopa {

riscv_trans::Register Id::value_to_riscv(std::string &str) const {
    if (id_type == koopa::id_type::FuncId) {
        str += to_riscv_style(*lit);
        return riscv_trans::Register();
    }
    else if (id_type == koopa::id_type::BlockLabel){
        /* empty */
        return riscv_trans::Register();
    }
    else if (id_type == koopa::id_type::GlobalId) {
        str += to_riscv_style(*lit);
        return riscv_trans::Register();
    }
    else {
        auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();
        str += riscv_trans::id_storage_map.get_storage(this)->get(target_reg);
        return target_reg;
    }
}

riscv_trans::Register Const::value_to_riscv(std::string &str) const {
    auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();
    str += build_inst("li", target_reg.get_lit(), std::to_string(val));
    return target_reg;
}

void ConstInitializer::initializer_to_riscv(std::string &str) const {
    str += "\t.word " + std::to_string(val) + '\n';
}

static bool is_commutative(Op op) {
    switch (op) {
        case op::EQ: case op::NE: case op::ADD: case op::MUL:
        case op::AND: case op::OR: case op::XOR:
            return true;
        default:
            return false;
    }
}

static bool has_i_type_inst(Op op) {
    switch (op) {
        case op::ADD: case op::AND: case op::OR: case op::XOR:
        case op::EQ: case op::NE:
            return true; 
        default:
            return false;
    }
}

Expr *exchanged_expr(const Expr *expr) {
    switch (expr->op) {
        case op::EQ: case op::NE: case op::ADD: case op::MUL: 
        case op::AND: case op::OR: case op::XOR:
            return new Expr(expr->op, expr->rv, expr->lv);
        case op::GT:
            return new Expr(op::LT, expr->rv, expr->lv);
        case op::LT:
            return new Expr(op::GT, expr->rv, expr->lv);
        case op::GE:
            return new Expr(op::LE, expr->rv, expr->lv);
        case op::LE:
            return new Expr(op::GE, expr->rv, expr->lv);
        default:
            throw "try to exchange a not commutative expression";
    }
}

riscv_trans::Register Expr::rvalue_to_riscv(std::string &str) const {
    riscv_trans::Register first_reg, second_reg, target_reg;
    std::string second_lit;
    bool is_i_type_inst = false;
    
    /* due to constant folding, it's impossible that lv->is_const && rv->is_const */
    if (lv->is_const && !rv->is_const && is_commutative(op) && has_i_type_inst(op)) {

        //! memory leak
        return exchanged_expr(this)->rvalue_to_riscv(str);
        
    }
    else if (!lv->is_const && rv->is_const && has_i_type_inst(op)) {
        
        is_i_type_inst = true;
        first_reg = lv->value_to_riscv(str);
        second_lit = std::to_string(rv->val);

    }
    else {

        first_reg = lv->value_to_riscv(str);
        second_reg = rv->value_to_riscv(str);
        second_lit = second_reg.get_lit();

    }

    target_reg = riscv_trans::temp_reg_manager.get_unused_reg();

    switch (op) {
        case op::NE: {
            str += build_inst("xor", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            str += build_inst("snez", target_reg.get_lit(), target_reg.get_lit());
            break;
        }
        case op::EQ: {
            str += build_inst("xor", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            str += build_inst("seqz", target_reg.get_lit(), target_reg.get_lit());
            break;
        }
        case op::GT: {
            str += build_inst("sgt", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
        case op::LT: {
            str += build_inst("slt", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
        case op::GE: {
            str += build_inst("slt", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            str += build_inst("xori", target_reg.get_lit(), target_reg.get_lit(), "1");
            break;
        }
        case op::LE: {
            str += build_inst("sgt", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            str += build_inst("xori", target_reg.get_lit(), target_reg.get_lit(), "1");
            break;
        }
        case op::ADD: {
            str += build_inst("add", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
        case op::SUB: {
            str += build_inst("sub", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
        case op::MUL: {
            str += build_inst("mul", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
        case op::DIV: {
            str += build_inst("div", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
        case op::MOD: {
            str += build_inst("rem", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
        case op::AND: {
            str += build_inst("and", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
        case op::OR: {
            str += build_inst("or", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
        case op::XOR: {
            str += build_inst("xor", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
        case op::SHL: {
            str += build_inst("sll", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
        case op::SHR: {
            str += build_inst("srl", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
        case op::SAR: {
            str += build_inst("sra", target_reg.get_lit(), first_reg.get_lit(), second_lit, is_i_type_inst);
            break;
        }
    }

    riscv_trans::temp_reg_manager.refresh_reg(first_reg);
    if (!is_i_type_inst) riscv_trans::temp_reg_manager.refresh_reg(second_lit);

    return target_reg;
}

riscv_trans::Register MemoryDecl::rvalue_to_riscv(std::string &str) const {
    return riscv_trans::Register();
}

riscv_trans::Register Load::rvalue_to_riscv(std::string &str) const {
    auto target_reg = riscv_trans::temp_reg_manager.get_unused_reg();
    str += riscv_trans::id_storage_map.get_storage(addr)->get(target_reg);
    return target_reg;
}

void StoreValue::stmt_to_riscv(std::string &str, riscv_trans::TransMode trans_mode) const {

    str += build_comment(this);

    auto val_reg = value->value_to_riscv(str);

    str += riscv_trans::id_storage_map.get_storage(addr)->save(val_reg);

    riscv_trans::temp_reg_manager.refresh_reg(val_reg);
}

void SymbolDef::stmt_to_riscv(std::string &str, riscv_trans::TransMode trans_mode) const {

    str += build_comment(this);

    //! ugly and not safe
    if (typeid(*val) == typeid(MemoryDecl)) return;

    auto source_reg = val->rvalue_to_riscv(str);

    str += riscv_trans::id_storage_map.get_storage(id)->save(source_reg);

    riscv_trans::temp_reg_manager.refresh_reg(source_reg);
}

void Return::stmt_to_riscv(std::string &str, riscv_trans::TransMode trans_mode) const {

    str += build_comment(this);

    if (return_type == return_type::HasRetVal) {
        auto ret_val_reg = val->value_to_riscv(str);
        str += build_inst("mv", "a0", ret_val_reg.get_lit());
        riscv_trans::temp_reg_manager.refresh_reg(ret_val_reg);
    }

    if (riscv_trans::current_has_called_func) {
        str += build_inst(
            "lw", "ra", std::to_string(riscv_trans::current_stack_frame_size - 4) + "(sp)"
        );
    }

    if (riscv_trans::current_stack_frame_size != 0) {
        str += build_inst(
            "addi", "sp", "sp", 
            std::to_string(riscv_trans::current_stack_frame_size)
        );
    }

    str += build_inst("ret");
}

void Branch::stmt_to_riscv(std::string &str, riscv_trans::TransMode trans_mode) const {
    auto cond_reg = cond->value_to_riscv(str);
    str += build_inst("bnez", cond_reg.get_lit(), to_riscv_style(*target1->lit));
    str += build_inst("j", to_riscv_style(*target2->lit));

    riscv_trans::temp_reg_manager.refresh_reg(cond_reg);
}

void Jump::stmt_to_riscv(std::string &str, riscv_trans::TransMode trans_mode) const {
    str += build_inst("j", to_riscv_style(*target->lit));
}

riscv_trans::Register FuncCall::rvalue_to_riscv(std::string &str) const {
    // TODO
    return riscv_trans::Register();
}

void FuncCall::stmt_to_riscv(std::string &str, riscv_trans::TransMode trans_mode) const {
    str += build_comment(dynamic_cast<const koopa::Stmt *>(this));

    // TODO
}

void Block::block_to_riscv(std::string &str) const {
    str += to_riscv_style(*id->lit) + ":\n";
    for(auto stmt: stmts) {
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

void FuncDef::stmt_to_riscv(std::string &str, riscv_trans::TransMode trans_mode) const {
    if (trans_mode == riscv_trans::trans_mode::TextSegment) {

        str += build_comment(this);
        
        value_manager.enter_func(*id->lit);

        riscv_trans::allocate_ids_storage_location(this);

        id->value_to_riscv(str);
        str += ":\n";

        if (riscv_trans::current_stack_frame_size != 0) {
            str += build_inst(
                "addi", "sp", "sp", 
                '-' + std::to_string(riscv_trans::current_stack_frame_size));
        }

        if (riscv_trans::current_has_called_func) {
            str += build_inst(
                "sw", "ra", std::to_string(riscv_trans::current_stack_frame_size - 4) + "(sp)"
            );
        }

        for (auto block: blocks) {
            block->block_to_riscv(str);
        }

        value_manager.leave_func();
    }
}

void GlobalSymbolDef::stmt_to_riscv(std::string &str, riscv_trans::TransMode trans_mode) const {
    if (trans_mode == riscv_trans::trans_mode::DataSegment) {
        riscv_trans::id_storage_map.register_id(
            id, 
            new riscv_trans::DataSeg(to_riscv_style(*id->lit))
        );

        str += build_comment(this);

        str += "\t.global ";
        id->value_to_riscv(str);
        str += '\n';

        id->value_to_riscv(str);
        str += ":\n";

        decl->stmt_to_riscv(str, trans_mode);
    }
}

void GlobalMemoryDecl::stmt_to_riscv(std::string &str, riscv_trans::TransMode trans_mode) const {
    initializer->initializer_to_riscv(str);
}

void FuncDecl::stmt_to_riscv(std::string &str, riscv_trans::TransMode trans_mode) const {
}

void Program::prog_to_riscv(std::string &str) const {
    str += "\t.data\n";
    for (auto global_stmt: global_stmts) {
        global_stmt->stmt_to_riscv(str, riscv_trans::trans_mode::DataSegment);
    }


    str += "\t.text\n";
    str += "\t.global main\n";
    for (auto global_stmt: global_stmts) {
        global_stmt->stmt_to_riscv(str, riscv_trans::trans_mode::TextSegment);
    }

    
}

}