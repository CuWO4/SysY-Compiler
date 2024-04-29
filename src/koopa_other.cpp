#include "../include/koopa.h"

#include <iostream>

namespace koopa {

void operator+=(Block& self, Stmt* stmt) {
    self.stmts.push_back(stmt);
}

void operator+=(Block& self, std::vector<Stmt*> stmts) {
    self.stmts.reserve(self.stmts.size() + stmts.size());
    self.stmts.insert(self.stmts.end(), stmts.begin(), stmts.end());
}

bool NotEndStmt::is_end_stmt() { return false; }
bool EndStmt::is_end_stmt() { return true; }
bool GlobalStmt::is_end_stmt() { return false; }

TypeId Int::get_type_id() { return type::Int; }
TypeId Array::get_type_id() { return type::Array; }
TypeId Pointer::get_type_id() { return type::Pointer; }
TypeId FuncType::get_type_id() { return type::FuncType; }
TypeId Label::get_type_id() { return type::Label; }
TypeId Void::get_type_id() { return type::Void; }

bool Type::operator!=(Type& other) {
    return !(*this == other);
}

bool Int::operator==(Type& other) { 
    return other.get_type_id() == type::Int; 
}

bool Array::operator==(Type& other) {
    return other.get_type_id() == type::Array 
        && dynamic_cast<Array&>(other).length == length;
}

bool Pointer::operator==(Type& other) {
    return other.get_type_id() == type::Pointer
        && *dynamic_cast<Pointer&>(other).pointed_type == *pointed_type;
}

bool FuncType::operator==(Type& other) {
    if (other.get_type_id() != type::FuncType) return false;
    auto other_casted = dynamic_cast<FuncType&>(other); 
    if (* other_casted.ret_type != *ret_type) return false;
    if (arg_types.size() != other_casted.arg_types.size()) return false;
    for (int i = 0; i < arg_types.size(); i++) {
        if (* arg_types[i] != *other_casted.arg_types[i]) return false;
    }
    return true;
}

bool Void::operator==(Type& other) {
    return other.get_type_id() == type::Void;
}

bool Label::operator==(Type& other) {
    return other.get_type_id() == type::Label;
}

bool Id::is_const() {
    return is_const_bool;
}

bool Const::is_const() {
    return true;
}

bool Undef::is_const() {
    return false;
}

int Id::get_val() {
    assert(is_const());
    return val;
}

int Const::get_val() {
    return val;
}

int Undef::get_val() {
    assert(is_const());
    return 0;
}

unsigned Type::get_byte_size() const {
    std::cerr << "try to get byte size of an unproper type" << std::endl;
    assert(0);
    return 0;
}

unsigned Int::get_byte_size() const { return 1; }

unsigned Array::get_byte_size() const {
    return length * elem_type->get_byte_size();
}

unsigned Pointer::get_byte_size() const { return 1; }

std::vector<int> Type::get_dim() const {
    assert(0);
    return {};
}

std::vector<int> Int::get_dim() const {
    return {};
}

std::vector<int> Array::get_dim() const {
    auto res = elem_type->get_dim();
    res.insert(res.begin(), length);
    return res;
}

std::vector<int> Pointer::get_dim() const {
    auto res = pointed_type->get_dim();
    res.insert(res.begin(), -1);
    return res;
}

}