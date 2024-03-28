#include "../include/koopa.h"

namespace koopa {

void operator+=(Block &self, Stmt *stmt) {
    self.stmts.push_back(stmt);
}

void operator+=(Block &self, std::vector<Stmt *> stmts) {
    self.stmts.reserve(self.stmts.size() + stmts.size());
    self.stmts.insert(self.stmts.end(), stmts.begin(), stmts.end());
}

namespace op {
    std::function<int(int, int)> op_func[] = {
        [] (int a, int b) { return a != b; },
        [] (int a, int b) { return a == b; },
        [] (int a, int b) { return a > b; },
        [] (int a, int b) { return a < b; },
        [] (int a, int b) { return a >= b; },
        [] (int a, int b) { return a <= b; },
        [] (int a, int b) { return a + b; },
        [] (int a, int b) { return a - b; },
        [] (int a, int b) { return a * b; },
        [] (int a, int b) { return a / b; },
        [] (int a, int b) { return a % b; },
        [] (int a, int b) { return a & b; },
        [] (int a, int b) { return a | b; },
        [] (int a, int b) { return a ^ b; },
        [] (int a, int b) { return a << b; },
        [] (int a, int b) { return (unsigned)a >> (unsigned)b; },
        [] (int a, int b) { return a >> b; },
    };
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

bool Type::operator!=(Type &other) {
    return !(*this == other);
}

bool Int::operator==(Type &other) { 
    return other.get_type_id() == type::Int; 
}

bool Array::operator==(Type &other) {
    return other.get_type_id() == type::Array 
        && dynamic_cast<Array &>(other).length == length;
}

bool Pointer::operator==(Type &other) {
    return other.get_type_id() == type::Pointer
        && *dynamic_cast<Pointer &>(other).pointed_type == *pointed_type;
}

bool FuncType::operator==(Type &other) {
    if (other.get_type_id() != type::FuncType) return false;
    auto other_casted = dynamic_cast<FuncType &>(other); 
    if (*other_casted.ret_type != *ret_type) return false;
    if (arg_types.size() != other_casted.arg_types.size()) return false;
    for (int i = 0; i < arg_types.size(); i++) {
        if (*arg_types[i] != *other_casted.arg_types[i]) return false;
    }
    return true;
}

bool Void::operator==(Type &other) {
    return other.get_type_id() == type::Void;
}

bool Label::operator==(Type &other) {
    return other.get_type_id() == type::Label;
}

}