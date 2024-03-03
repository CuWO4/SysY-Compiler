#include "../include/koopa.h"

namespace koopa {

Array::Array(Type *elem_type, int length) : elem_type(elem_type), length(length) {
    elem_type->pa = this;
}

Pointer::Pointer(Type *pointed_type) : pointed_type(pointed_type) {
    pointed_type->pa = this;
}

FuncType::FuncType(std::vector<Type *> arg_types, Type *ret_type)
    : arg_types(arg_types), ret_type(ret_type) {
    for (auto arg_type : arg_types) arg_type->pa = this;
    ret_type->pa = this;
}

Id::Id(Type *type, std::string *lit, bool is_const, int val) 
    : type(type), lit(lit) {
    this->val = val;
    this->is_const = is_const;

    type->pa = this;
}

Const::Const(int val)  {
    this->val = val;
    this->is_const = true;
}

ConstInitializer::ConstInitializer(int val) : val(val) {}

Aggregate::Aggregate(std::vector<Initializer *> initializers) 
    : initializers(initializers) {
    for (auto initializer : initializers) initializer->pa = this;
}

MemoryDecl::MemoryDecl(Type *type) : type(type) {
    type->pa = this;
}

Load::Load(Id *addr) : addr(addr) {
    addr->pa = this;
}

GetPtr::GetPtr(Id *base, Value *offset) : base(base), offset(offset) {
    base->pa = this;
    offset->pa = this;
}

GetElemPtr::GetElemPtr(Id *base, Value *offset) : base(base), offset(offset) {
    base->pa = this;
    offset->pa = this;
}

Expr::Expr(op::Op op, Value *lv, Value *rv, bool is_const) 
    : op(op), lv(lv), rv(rv), is_const(is_const) {
    lv->pa = this;
    rv->pa = this;
}

ExprStmt::ExprStmt(Expr *expr) : expr(expr) {}

FuncCall::FuncCall(Id *id, std::vector<Value *> args) : id(id), args(args) {
    auto base_this = this;

    id->pa = base_this;
    for (auto arg : args) arg->pa = base_this;
}

SymbolDef::SymbolDef(Id *id, Rvalue *val) : id(id), val(val) {
    id->pa = this;
    val->pa = this;

    is_unit = false;
}

StoreValue::StoreValue(Value *value, Id *addr) : value(value), addr(addr) {
    value->pa = this;
    addr->pa = this;
}

StoreInitializer::StoreInitializer(Initializer *initializer, Id *addr)
    : initializer(initializer), addr(addr) {

    initializer->pa = this;
    addr->pa = this;
}

Branch::Branch(Value *cond, Id *target1, Id *target2) 
    : cond(cond), target1(target1), target2(target2) {

    cond->pa = this;
    target1->pa = this;
    target2->pa = this;
}

Jump::Jump(Id *target) : target(target) {
    target->pa = this;
}

Return::Return() : has_return_val(false) {}
Return::Return(Value *val) : val(val) {
    val->pa = this;
}

Block::Block(Id *id, std::vector<Stmt *> stmts)
    : id(id), stmts(stmts) {

    id->pa = this;
    for (auto stmt : stmts) stmt->pa = this;
}

FuncParamDecl::FuncParamDecl(Id *id, Type *type)
    : id(id), type(type) {

    id->pa = this;
    type->pa = this;
}

FuncDef::FuncDef(Id *id, std::vector<FuncParamDecl *> func_param_decls,
                 Type *ret_type, std::vector<Block *> blocks)
    : id(id), func_param_decls(func_param_decls), ret_type(ret_type), blocks(blocks) {

    id->pa = this;
    for (auto func_param_decl : func_param_decls) func_param_decl->pa = this;
    ret_type->pa = this;
    for (auto block : blocks) block->pa = this;
}

FuncDecl::FuncDecl(Id *id, std::vector<Type *> param_types, Type *ret_type)
    : id(id), param_types(param_types), ret_type(ret_type) {

    id->pa = this;
    for (auto param_type : param_types) param_type->pa = this;
    ret_type->pa = this;
}

GlobalMemoryDecl::GlobalMemoryDecl(Type *type, Initializer *initializer)
    : type(type), initializer(initializer) {

    type->pa = this;
    initializer->pa = this;
}

GlobalSymbolDef::GlobalSymbolDef(Id *id, GlobalMemoryDecl *decl)
    : id(id), decl(decl) {

    id->pa = this;
    decl->pa = this;
}

Program::Program(std::vector<GlobalStmt *> global_stmts)
    : global_stmts(global_stmts) {

    for (auto global_stmt : global_stmts) global_stmt->pa = this;

}

}