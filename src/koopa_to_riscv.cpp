#include "../include/koopa.h"
#include "../include/assign.h"
#include "../include/def.h"

#include <string>

static std::string to_riscv_style(std::string symbol) {
    return symbol.substr(1, symbol.length() - 1);
}

static std::string build_inst(std::string op_code, 
                              std::string r1 = {}, std::string r2 = {}, std::string r3 = {}, 
                              bool is_i_type_inst = false) {
    auto res = '\t' + assign(op_code + (is_i_type_inst ? "i" : ""));
    if (r1 != "") res += r1;
    if (r2 != "") res += ", " + r2;
    if (r3 != "") res += ", " + r3;
    res += '\n';
    return res;
}

namespace koopa {

static std::string build_comment(const Base *obj) {
    return debug_mode_riscv ? "\t# " + obj->to_string() + '\n' : "";
}

static std::string build_mem(int offset, std::string base = "sp") { 
    return std::to_string(offset) + '(' + base + ')';
}

void Id::to_riscv(std::string &str, riscv_trans::Info &info) const {
    if ((typeid(*type) == typeid(FuncType))) {
        str += to_riscv_style(*lit);
    }
    else if (typeid(*type) == typeid(Label)){
        /* empty */
    }
    else if (typeid(*type) == typeid(Void)) {
        /* empty */
    }
    else {
        auto target_reg = info.get_unused_reg();
        str += build_inst("lw", target_reg, build_mem(sf_offset));
        info.res_lit = target_reg;
    }
}

void Const::to_riscv(std::string &str, riscv_trans::Info &info) const {
    auto target = info.get_unused_reg();
    str += build_inst("li", target, std::to_string(val));
    info.res_lit = target;
}

static bool is_commutative(op::Op op) {
    switch (op) {
        case op::EQ: case op::NE: case op::ADD: case op::MUL:
        case op::AND: case op::OR: case op::XOR:
            return true;
        default:
            return false;
    }
}

static bool has_i_type_inst(op::Op op) {
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

void Expr::to_riscv(std::string &str, riscv_trans::Info &info) const {
    std::string first_reg, second_lit, target_reg;
    bool is_i_type_inst = false;
    
    /* due to constant folding, it's impossible that lv->is_const && rv->is_const */
    if (lv->is_const && !rv->is_const && is_commutative(op) && has_i_type_inst(op)) {

        //! memory leak
        exchanged_expr(this)->to_riscv(str, info);
        return;
        
    }
    else if (!lv->is_const && rv->is_const && has_i_type_inst(op)) {
        
        is_i_type_inst = true;

        lv->to_riscv(str, info);
        first_reg = info.res_lit;
        
        second_lit = std::to_string(rv->val);

    }
    else {

        lv->to_riscv(str, info);
        first_reg = info.res_lit;

        rv->to_riscv(str, info);
        second_lit = info.res_lit;

    }

    target_reg = info.get_unused_reg();

    switch (op) {
        case op::NE: {
            str += build_inst("xor", target_reg, first_reg, second_lit, is_i_type_inst);
            str += build_inst("snez", target_reg, target_reg);
            break;
        }
        case op::EQ: {
            str += build_inst("xor", target_reg, first_reg, second_lit, is_i_type_inst);
            str += build_inst("seqz", target_reg, target_reg);
            break;
        }
        case op::GT: {
            str += build_inst("sgt", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case op::LT: {
            str += build_inst("slt", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case op::GE: {
            str += build_inst("slt", target_reg, first_reg, second_lit, is_i_type_inst);
            str += build_inst("xori", target_reg, target_reg, "1");
            break;
        }
        case op::LE: {
            str += build_inst("sgt", target_reg, first_reg, second_lit, is_i_type_inst);
            str += build_inst("xori", target_reg, target_reg, "1");
            break;
        }
        case op::ADD: {
            str += build_inst("add", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case op::SUB: {
            str += build_inst("sub", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case op::MUL: {
            str += build_inst("mul", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case op::DIV: {
            str += build_inst("div", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case op::MOD: {
            str += build_inst("rem", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case op::AND: {
            str += build_inst("and", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case op::OR: {
            str += build_inst("or", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case op::XOR: {
            str += build_inst("xor", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case op::SHL: {
            str += build_inst("sll", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case op::SHR: {
            str += build_inst("srl", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case op::SAR: {
            str += build_inst("sra", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
    }

    info.refresh_reg(first_reg);
    if (!is_i_type_inst) info.refresh_reg(second_lit);

    info.res_lit = target_reg;
}

void ExprStmt::to_riscv(std::string &str, riscv_trans::Info &info) const {

    str += build_comment(this);

    expr->to_riscv(str, info);

    info.refresh_reg(info.res_lit);
}

void MemoryDecl::to_riscv(std::string &str, riscv_trans::Info &info) const {
}

void Load::to_riscv(std::string &str, riscv_trans::Info &info) const {
    auto target_reg = info.get_unused_reg();

    str += build_inst("lw", target_reg, build_mem(addr->sf_offset));

    info.res_lit = target_reg;
}

void StoreValue::to_riscv(std::string &str, riscv_trans::Info &info) const {

    str += build_comment(this);

    value->to_riscv(str, info);
    auto val_reg = info.res_lit;

    str += build_inst("sw", val_reg, build_mem(addr->sf_offset));

    info.refresh_reg(val_reg);
}

void SymbolDef::to_riscv(std::string &str, riscv_trans::Info &info) const {

    str += build_comment(this);

    if (typeid(*val) == typeid(MemoryDecl)) return;

    val->to_riscv(str, info);
    auto source_reg = info.res_lit;

    str += build_inst("sw", source_reg, build_mem(id->sf_offset));

    info.refresh_reg(source_reg);
}

void Return::to_riscv(std::string &str, riscv_trans::Info &info) const {

    str += build_comment(this);

    if (has_return_val) {

        val->to_riscv(str, info);
        str += build_inst("mv", "a0", info.res_lit);
        info.refresh_reg(info.res_lit);
        
    }

    if (info.stack_frame_size != 0) {
        str += build_inst("addi", "sp", "sp", std::to_string(info.stack_frame_size));
    }

    str += build_inst("ret");
}

void Branch::to_riscv(std::string &str, riscv_trans::Info &info) const {
    cond->to_riscv(str, info);
    str += build_inst("bnez", info.res_lit, to_riscv_style(*target1->lit));
    str += build_inst("j", to_riscv_style(*target2->lit));

    info.refresh_reg(info.res_lit);
}

void Jump::to_riscv(std::string &str, riscv_trans::Info &info) const {
    str += build_inst("j", to_riscv_style(*target->lit));
}

void Block::to_riscv(std::string &str, riscv_trans::Info &info) const {
    str += to_riscv_style(*id->lit) + ":\n";
    for(auto stmt : stmts) {
        stmt->to_riscv(str, info);
    }
}

void FuncDef::to_riscv(std::string &str, riscv_trans::Info &info) const {
    // TODO
    id->to_riscv(str, info);
    str += ":\n";

    int stack_frame_size = 0;
    for (auto block : blocks) {
        stack_frame_size += block->get_stack_frame_size();
    }

    if (stack_frame_size != 0) {
        int start_offset = stack_frame_size;
        for (auto block : blocks) {
            block->set_id_offset(start_offset);
        }

        stack_frame_size = (stack_frame_size / 16 + 1) * 16;
        info.stack_frame_size = stack_frame_size;
        str += build_inst("addi", "sp", "sp", '-' + std::to_string(stack_frame_size));
    }

    for (auto block : blocks) {
        block->to_riscv(str, info);
    }

    
}

void Program::to_riscv(std::string &str, riscv_trans::Info &info) const {
    str += "\t.text\n";
    str += "\t.global main\n";

    for (auto global_stmt : global_stmts) {
        global_stmt->to_riscv(str, info);
    }

    
}

}