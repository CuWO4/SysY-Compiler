#include "koopa.h"

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

Type::TypeId Int::get_type_id() { return Type::Int; }
Type::TypeId Array::get_type_id() { return Type::Array; }
Type::TypeId Pointer::get_type_id() { return Type::Pointer; }
Type::TypeId FuncType::get_type_id() { return Type::FuncType; }
Type::TypeId Void::get_type_id() { return Type::Void; }

bool Type::operator==(Type& other) {
    return *this == std::move(other);
}

bool Type::operator!=(Type& other) {
    return !(*this == other);
}

bool Type::operator!=(Type&& other) {
    return !(*this == other);
}

bool Int::operator==(Type&& other) { 
    return other.get_type_id() == Type::Int; 
}

bool Array::operator==(Type&& other) {
    return other.get_type_id() == Type::Array 
        && dynamic_cast<Array&>(other).length == length;
}

bool Pointer::operator==(Type&& other) {
    return other.get_type_id() == Type::Pointer
        && *dynamic_cast<Pointer&>(other).pointed_type == *pointed_type;
}

bool FuncType::operator==(Type&& other) {
    if (other.get_type_id() != Type::FuncType) return false;
    
    auto other_casted = dynamic_cast<FuncType&>(other); 

    if (*other_casted.ret_type != *ret_type) return false;

    if (arg_types.size() != other_casted.arg_types.size()) return false;

    for (int i = 0; i < arg_types.size(); i++) {
        if (*arg_types[i] != *other_casted.arg_types[i]) return false;
    }

    return true;
}

bool Void::operator==(Type&& other) {
    return other.get_type_id() == Type::Void;
}

std::vector<Type*> FuncType::get_arg_types() const { return arg_types; };
Type* FuncType::get_ret_type() const { return ret_type; }

Type* Id::get_type() const { return type; }
std::string Id::get_lit() const { return lit; }

Id* FuncDef::get_id() const { return id; }
std::vector<Id*> FuncDef::get_formal_param_ids() const { return formal_param_ids; }
std::vector<Block*> FuncDef::get_blocks() const { return blocks; }

int ConstInitializer::get_val() const { return val; }

Id* FuncCall::get_id() const { return id;}
std::vector<Value*> FuncCall::get_args() const { return args; };

Rvalue* SymbolDef::get_val() const { return val; }

Label Block::get_label() const { return label;}
std::vector<Stmt*>& Block::get_stmts() { return stmts; }

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

unsigned Int::get_byte_size() const { return 4; }

unsigned Array::get_byte_size() const {
    return length * elem_type->get_byte_size();
}

unsigned Pointer::get_byte_size() const { return 4; }

std::vector<int> ConstInitializer::to_flat_vec(unsigned byte_size) const {
    return { val };
}

std::vector<int> Aggregate::to_flat_vec(unsigned byte_size) const {
    auto res = std::vector<int>();
    res.reserve(byte_size / 4);

    for (auto initializer: initializers) {
        auto sub_flat_vec = initializer->to_flat_vec(byte_size / initializers.size());
        res.insert(res.end(), sub_flat_vec.begin(), sub_flat_vec.end());
    }
    
    return res;
}

std::vector<int> Zeroinit::to_flat_vec(unsigned byte_size) const {
    return std::vector<int>(byte_size / 4, 0);
}



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

koopa::Type* Type::unwrap() const {
    assert(0);
    return nullptr;
}

koopa::Type* Array::unwrap() const {
    return elem_type;
}

koopa::Type* Pointer::unwrap() const {
    return pointed_type;
}

}