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

static std::string build_comment(const koopa::Base *obj) {
    return debug_mode_riscv ? "\t# " + obj->to_string() + '\n' : "";
}

static std::string build_mem(int offset, std::string base = "sp") { 
    return std::to_string(offset) + '(' + base + ')';
}

void koopa::Id::to_riscv(std::string &str, riscv_trans::Info &info) const {
    if ((typeid(*type) == typeid(koopa::FuncType))) {
        str += to_riscv_style(*lit);
    }
    else if (typeid(*type) == typeid(koopa::Label)){
        //TODO
    }
    else if (typeid(*type) == typeid(koopa::Void)) {
        //TODO
    }
    else {
        auto target_reg = info.get_unused_reg();
        str += build_inst("lw", target_reg, build_mem(sf_offset));
        info.res_lit = target_reg;
    }
}

void koopa::Const::to_riscv(std::string &str, riscv_trans::Info &info) const {
    auto target = info.get_unused_reg();
    str += build_inst("li", target, std::to_string(val));
    info.res_lit = target;
}

static bool is_commutative(koopa::op::Op op) {
    switch (op) {
        case koopa::op::EQ: case koopa::op::NE: case koopa::op::GT: case koopa::op::LT: 
        case koopa::op::GE: case koopa::op::LE: case koopa::op::ADD: case koopa::op::MUL:
        case koopa::op::AND: case koopa::op::OR: case koopa::op::XOR:
            return true;
        default:
            return false;
    }
}

static bool has_i_type_inst(koopa::op::Op op) {
    switch (op) {
        case koopa::op::MUL: case koopa::op::MOD:
            return false;
        default:
            return true; 
    }
}

koopa::Expr *exchanged_expr(const koopa::Expr *expr) {
    switch (expr->op) {
        case koopa::op::EQ: case koopa::op::NE: case koopa::op::ADD: case koopa::op::MUL: 
        case koopa::op::AND: case koopa::op::OR: case koopa::op::XOR:
            return new koopa::Expr(expr->op, expr->rv, expr->lv);
        case koopa::op::GT:
            return new koopa::Expr(koopa::op::LT, expr->rv, expr->lv);
        case koopa::op::LT:
            return new koopa::Expr(koopa::op::GT, expr->rv, expr->lv);
        case koopa::op::GE:
            return new koopa::Expr(koopa::op::LE, expr->rv, expr->lv);
        case koopa::op::LE:
            return new koopa::Expr(koopa::op::GE, expr->rv, expr->lv);
        default:
            throw "try to exchange a not commutative expression";
    }
}

void koopa::Expr::to_riscv(std::string &str, riscv_trans::Info &info) const {
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
        case koopa::op::NE: {
            str += build_inst("xor", target_reg, first_reg, second_lit, is_i_type_inst);
            str += build_inst("snez", target_reg, target_reg);
            break;
        }
        case koopa::op::EQ: {
            str += build_inst("xor", target_reg, first_reg, second_lit, is_i_type_inst);
            str += build_inst("seqz", target_reg, target_reg);
            break;
        }
        case koopa::op::GT: {
            str += build_inst("sgt", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case koopa::op::LT: {
            str += build_inst("slt", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case koopa::op::GE: {
            str += build_inst("slt", target_reg, first_reg, second_lit, is_i_type_inst);
            str += build_inst("xori", target_reg, target_reg, "1");
            break;
        }
        case koopa::op::LE: {
            str += build_inst("sgt", target_reg, first_reg, second_lit, is_i_type_inst);
            str += build_inst("xori", target_reg, target_reg, "1");
            break;
        }
        case koopa::op::ADD: {
            str += build_inst("add", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case koopa::op::SUB: {
            str += build_inst("sub", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case koopa::op::MUL: {
            str += build_inst("mul", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case koopa::op::DIV: {
            str += build_inst("div", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case koopa::op::MOD: {
            str += build_inst("rem", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case koopa::op::AND: {
            str += build_inst("and", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case koopa::op::OR: {
            str += build_inst("or", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case koopa::op::XOR: {
            str += build_inst("xor", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case koopa::op::SHL: {
            str += build_inst("sll", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case koopa::op::SHR: {
            str += build_inst("srl", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
        case koopa::op::SAR: {
            str += build_inst("sra", target_reg, first_reg, second_lit, is_i_type_inst);
            break;
        }
    }

    info.refresh_reg(first_reg);
    if (!is_i_type_inst) info.refresh_reg(second_lit);

    info.res_lit = target_reg;
}

void koopa::ExprStmt::to_riscv(std::string &str, riscv_trans::Info &info) const {

    str += build_comment(this);

    expr->to_riscv(str, info);

    info.refresh_reg(info.res_lit);
}

void koopa::MemoryDecl::to_riscv(std::string &str, riscv_trans::Info &info) const {
}

void koopa::Load::to_riscv(std::string &str, riscv_trans::Info &info) const {
    auto target_reg = info.get_unused_reg();

    str += build_inst("lw", target_reg, build_mem(addr->sf_offset));

    info.res_lit = target_reg;
}

void koopa::StoreValue::to_riscv(std::string &str, riscv_trans::Info &info) const {

    str += build_comment(this);

    value->to_riscv(str, info);
    auto val_reg = info.res_lit;

    str += build_inst("sw", val_reg, build_mem(addr->sf_offset));

    info.refresh_reg(val_reg);
}

void koopa::SymbolDef::to_riscv(std::string &str, riscv_trans::Info &info) const {

    str += build_comment(this);

    if (typeid(*val) == typeid(koopa::MemoryDecl)) return;

    val->to_riscv(str, info);
    auto source_reg = info.res_lit;

    str += build_inst("sw", source_reg, build_mem(id->sf_offset));

    info.refresh_reg(source_reg);
}

void koopa::Return::to_riscv(std::string &str, riscv_trans::Info &info) const {

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

void koopa::Block::to_riscv(std::string &str, riscv_trans::Info &info) const {
    for(auto stmt : stmts) {
        stmt->to_riscv(str, info);
    }
}

void koopa::FuncDef::to_riscv(std::string &str, riscv_trans::Info &info) const {

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

void koopa::Program::to_riscv(std::string &str, riscv_trans::Info &info) const {
    str += "\t.text\n";
    str += "\t.global main\n";

    for (auto global_stmt : global_stmts) {
        global_stmt->to_riscv(str, info);
    }

    
}