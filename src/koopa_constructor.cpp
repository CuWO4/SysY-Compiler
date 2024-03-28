#include "../include/koopa.h"

namespace koopa {

Array::Array(Type *elem_type, int length): elem_type(elem_type), length(length) {}

Pointer::Pointer(Type *pointed_type): pointed_type(pointed_type) {}

FuncType::FuncType(std::vector<Type *> arg_types, Type *ret_type)
    : arg_types(arg_types), ret_type(ret_type) {}

Id::Id(IdType id_type, Type *type, std::string *lit, bool is_const, int val) 
    : id_type(id_type), type(type), lit(lit) {
    this->val = val;
    this->is_const = is_const;

}

Const::Const(int val)  {
    this->val = val;
    this->is_const = true;
}

ConstInitializer::ConstInitializer(int val): val(val) {}

Aggregate::Aggregate(std::vector<Initializer *> initializers) 
    : initializers(initializers) {}

MemoryDecl::MemoryDecl(Type *type): type(type) {}

Load::Load(Id *addr): addr(addr) {}

GetPtr::GetPtr(Id *base, Value *offset): base(base), offset(offset) {}

GetElemPtr::GetElemPtr(Id *base, Value *offset): base(base), offset(offset) {}

Expr::Expr(Op op, Value *lv, Value *rv, bool is_const) 
    : op(op), lv(lv), rv(rv), is_const(is_const) {}

FuncCall::FuncCall(Id *id, std::vector<Value *> args): id(id), args(args) {}

SymbolDef::SymbolDef(Id *id, Rvalue *val): id(id), val(val) {

    is_unit = false;
}

StoreValue::StoreValue(Value *value, Id *addr): value(value), addr(addr) {}

StoreInitializer::StoreInitializer(Initializer *initializer, Id *addr)
    : initializer(initializer), addr(addr) {}

Branch::Branch(Value *cond, Id *target1, Id *target2) 
    : cond(cond), target1(target1), target2(target2) {}

Jump::Jump(Id *target): target(target) {}

Return::Return(): return_type(return_type::NotHasRetVal) {}
Return::Return(Value *val): return_type(return_type::HasRetVal), val(val) {}

Block::Block(Id *id, std::vector<Stmt *> stmts)
    : id(id), stmts(stmts) {}

FuncDef::FuncDef(Id *id, std::vector<Id *> formal_param_ids, std::vector<Block *> blocks)
    : id(id), formal_param_ids(formal_param_ids), blocks(blocks) {}

FuncDecl::FuncDecl(Id *id): id(id) {}

GlobalMemoryDecl::GlobalMemoryDecl(Type *type, Initializer *initializer)
    : type(type), initializer(initializer) {}

GlobalSymbolDef::GlobalSymbolDef(Id *id, GlobalMemoryDecl *decl)
    : id(id), decl(decl) {}

Program::Program(std::vector<GlobalStmt *> global_stmts)
    : global_stmts(global_stmts) {}

}