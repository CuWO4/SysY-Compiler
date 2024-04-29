#include "../include/koopa.h"

namespace koopa {

Array::Array(Type* elem_type, int length): elem_type(elem_type), length(length) {
    assert(elem_type);
}

Pointer::Pointer(Type* pointed_type): pointed_type(pointed_type) {
    assert(pointed_type);
}

FuncType::FuncType(std::vector<Type*> arg_types, Type* ret_type)
    : arg_types(arg_types), ret_type(ret_type) {
    assert(ret_type);
}

Id::Id(IdType id_type, Type* type, std::string lit) 
    : id_type(id_type), type(type), lit(lit), is_const_bool(false), val(0) {
    assert(type);
}

Id::Id(IdType id_type, Type* type, std::string lit, int val) 
    : id_type(id_type), type(type), lit(lit), is_const_bool(true), val(val) {
    assert(type);
}

Const::Const(int val): val(val) {}

ConstInitializer::ConstInitializer(int val): val(val) {}

Aggregate::Aggregate(std::vector<Initializer*> initializers) 
    : initializers(initializers) {}

MemoryDecl::MemoryDecl(Type* type): type(type) {
    assert(type);
}

Load::Load(Id* addr): addr(addr) {
    assert(addr);
}

GetPtr::GetPtr(Id* base, Value* offset): base(base), offset(offset) {
    assert(base); assert(offset);
}

GetElemPtr::GetElemPtr(Id* base, Value* offset): base(base), offset(offset) {
    assert(base); assert(offset);
}

Expr::Expr(Value* lv, Value* rv): lv(lv), rv(rv) {
    assert(lv); assert(rv);
}

Eq::Eq(Value* lv, Value* rv): Expr(lv, rv) {}
Ne::Ne(Value* lv, Value* rv): Expr(lv, rv) {}
Gt::Gt(Value* lv, Value* rv): Expr(lv, rv) {}
Lt::Lt(Value* lv, Value* rv): Expr(lv, rv) {}
Ge::Ge(Value* lv, Value* rv): Expr(lv, rv) {}
Le::Le(Value* lv, Value* rv): Expr(lv, rv) {}
Add::Add(Value* lv, Value* rv): Expr(lv, rv) {}
Sub::Sub(Value* lv, Value* rv): Expr(lv, rv) {}
Mul::Mul(Value* lv, Value* rv): Expr(lv, rv) {}
Div::Div(Value* lv, Value* rv): Expr(lv, rv) {}
Mod::Mod(Value* lv, Value* rv): Expr(lv, rv) {}
And::And(Value* lv, Value* rv): Expr(lv, rv) {}
Or::Or(Value* lv, Value* rv): Expr(lv, rv) {}
Xor::Xor(Value* lv, Value* rv): Expr(lv, rv) {}
Shl::Shl(Value* lv, Value* rv): Expr(lv, rv) {}
Shr::Shr(Value* lv, Value* rv): Expr(lv, rv) {}
Sar::Sar(Value* lv, Value* rv): Expr(lv, rv) {}

FuncCall::FuncCall(Id* id, std::vector<Value*> args): id(id), args(args) {
    assert(id);
}

SymbolDef::SymbolDef(Id* id, Rvalue* val): id(id), val(val) {
    assert(id); assert(val);
    
    is_unit = false;
}

StoreValue::StoreValue(Value* value, Id* addr): value(value), addr(addr) {
    assert(value); assert(addr);
}

StoreInitializer::StoreInitializer(Initializer* initializer, Id* addr)
    : initializer(initializer), addr(addr) {
    assert(initializer); assert(addr);
}

Branch::Branch(Value* cond, Id* target1, Id* target2) 
    : cond(cond), target1(target1), target2(target2) {
    assert(cond); assert(target1); assert(target2);
}

Jump::Jump(Id* target): target(target) {
    assert(target);
}

Return::Return(): return_type(return_type::NotHasRetVal) {}

Return::Return(Value* val): return_type(return_type::HasRetVal), val(val) {
    assert(val);
}

Block::Block(Id* id, std::vector<Stmt*> stmts)
    : id(id), stmts(stmts) {
    assert(id);
}

FuncDef::FuncDef(Id* id, std::vector<Id*> formal_param_ids, std::vector<Block*> blocks)
    : id(id), formal_param_ids(formal_param_ids), blocks(blocks) {
    assert(id);
}

FuncDecl::FuncDecl(Id* id): id(id) {
    assert(id);
}

GlobalMemoryDecl::GlobalMemoryDecl(Type* type, Initializer* initializer)
    : type(type), initializer(initializer) {
    assert(type);
}

GlobalSymbolDef::GlobalSymbolDef(Id* id, GlobalMemoryDecl* decl)
    : id(id), decl(decl) {
    assert(id); assert(decl);
}

Program::Program(std::vector<GlobalStmt*> global_stmts)
    : global_stmts(global_stmts) {}

}