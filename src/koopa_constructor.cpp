#include "../include/koopa.h"
#include "../include/value_manager.h"

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

Label::Label(): name() {}
Label::Label(std::string name): name(name) {}

Id::Id(Type* type, std::string lit) 
    : type(type), lit(lit), is_const_bool(false), val(0) {
    assert(type);
}

Id::Id(Type* type, std::string lit, int val) 
    : type(type), lit(lit), is_const_bool(true), val(val) {
    assert(type);
}

Const::Const(int val): val(val) {}

ConstInitializer::ConstInitializer(int val): val(val) {}

Aggregate::Aggregate(std::vector<Initializer*> initializers) 
    : initializers(initializers) {}

MemoryDecl::MemoryDecl(
    Type* type, std::string pseudo_lit, NestingInfo* nesting_info
): type(type) {
    assert(type);

    pseudo_id = value_manager.new_id(
        type,
        "!" + pseudo_lit,
        nesting_info
    );
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
}

StoreValue::StoreValue(Value* value, Id* addr): value(value), addr(addr) {
    assert(value); assert(addr);
}

StoreInitializer::StoreInitializer(Initializer* initializer, Id* addr)
    : initializer(initializer), addr(addr) {
    assert(initializer); assert(addr);
}

Branch::Branch(Value* cond, Label target1, Label target2) 
    : cond(cond), target1(target1), target2(target2) {
    assert(cond);
}

Jump::Jump(Label target): target(target) {}

Return::Return(): return_type(NotHasRetVal) {}

Return::Return(Value* val): return_type(HasRetVal), val(val) {
    assert(val);
}

Block::Block(Label label, std::vector<Stmt*> stmts)
    : label(label), stmts(stmts) {
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