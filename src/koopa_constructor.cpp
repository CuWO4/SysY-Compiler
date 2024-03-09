#include "../include/koopa.h"

namespace koopa {

Array::Array(Type *elem_type, int length) : elem_type(elem_type), length(length) {}

Pointer::Pointer(Type *pointed_type) : pointed_type(pointed_type) {}

FuncType::FuncType(std::vector<Type *> arg_types, Type *ret_type)
    : arg_types(arg_types), ret_type(ret_type) {}

Id::Id(Type *type, std::string *lit, bool is_const, int val, bool is_formal_param) 
    : type(type), lit(lit), is_formal_param(is_formal_param) {
    this->val = val;
    this->is_const = is_const;
}

Const::Const(int val)  {
    this->val = val;
    this->is_const = true;
}

ConstInitializer::ConstInitializer(int val) : val(val) {}

Aggregate::Aggregate(std::vector<Initializer *> initializers) 
    : initializers(initializers) {}

MemoryDecl::MemoryDecl(Type *type) : type(type) {}

Load::Load(Id *addr) : addr(addr) {}

GetPtr::GetPtr(Id *base, Value *offset) : base(base), offset(offset) {}

GetElemPtr::GetElemPtr(Id *base, Value *offset) : base(base), offset(offset) {}

Expr::Expr(op::Op op, Value *lv, Value *rv, bool is_const) 
    : op(op), lv(lv), rv(rv), is_const(is_const) {}

ExprStmt::ExprStmt(Expr *expr) : expr(expr) {}

FuncCall::FuncCall(Id *id, std::vector<Value *> args) : id(id), args(args) {}

SymbolDef::SymbolDef(Id *id, Rvalue *val) : id(id), val(val) {

    is_unit = false;
}

StoreValue::StoreValue(Value *value, Id *addr) : value(value), addr(addr) {}

StoreInitializer::StoreInitializer(Initializer *initializer, Id *addr)
    : initializer(initializer), addr(addr) {}

Branch::Branch(Value *cond, Id *target1, Id *target2) 
    : cond(cond), target1(target1), target2(target2) {}

Jump::Jump(Id *target) : target(target) {}

Return::Return() : has_return_val(false) {}
Return::Return(Value *val) : val(val) {}

Block::Block(Id *id, std::vector<Stmt *> stmts)
    : id(id), stmts(stmts) {}

FuncDef::FuncDef(Id *id, std::vector<Id *> formal_param_ids,
                 Type *ret_type, std::vector<Block *> blocks)
    : id(id), formal_param_ids(formal_param_ids), ret_type(ret_type), blocks(blocks) {}

FuncDecl::FuncDecl(Id *id, Type *ret_type)
    : id(id), ret_type(ret_type) {}

GlobalMemoryDecl::GlobalMemoryDecl(Type *type, Initializer *initializer)
    : type(type), initializer(initializer) {}

GlobalSymbolDef::GlobalSymbolDef(Id *id, GlobalMemoryDecl *decl)
    : id(id), decl(decl) {}

Program::Program(std::vector<GlobalStmt *> global_stmts)
    : global_stmts(global_stmts) {}

}