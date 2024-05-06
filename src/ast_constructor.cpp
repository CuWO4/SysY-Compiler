#include "ast.h"

namespace ast {

BinaryExpr::BinaryExpr(Expr* lv, Expr* rv)
    : lv(lv), rv(rv) {
    assert(lv); assert(rv);
}

LogicOr::LogicOr(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
LogicAnd::LogicAnd(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Eq::Eq(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Neq::Neq(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Lt::Lt(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Gt::Gt(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Leq::Leq(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Geq::Geq(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Add::Add(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Sub::Sub(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Mul::Mul(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Div::Div(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Mod::Mod(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Comma::Comma(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}
Assign::Assign(Expr* lv, Expr* rv): BinaryExpr(lv, rv) {}

UnaryExpr::UnaryExpr(Expr* lv)
    : lv(lv) {
    assert(lv);
}

Neg::Neg(Expr* lv): UnaryExpr(lv) {}
Pos::Pos(Expr* lv): UnaryExpr(lv) {}
Not::Not(Expr* lv): UnaryExpr(lv) {}

Indexing::Indexing(Id* id, std::vector<Expr*> indexes)
    : id(id), indexes(indexes) {}

Pointer::Pointer(Type* pointed_type): pointed_type(pointed_type) {}

Array::Array(Type* elem_type, Expr* length)
    : element_type(elem_type), length(length) {}

ConstInitializer::ConstInitializer(Expr* val): val(val) {}

Aggregate::Aggregate(std::vector<Initializer*> initializers)
    : initializers(initializers) {}

Id::Id(std::string lit, NestingInfo* nesting_info)
    : lit(lit), nesting_info(nesting_info) {
    assert(nesting_info);
}

FuncCall::FuncCall(Id* func_id, std::vector<Expr*> actual_params) 
    : func_id(func_id), actual_params(actual_params) {
    assert(func_id);
}

Number::Number(int val): val(val) {}

VarDef::VarDef(Type* type, Id* id)
    : type(type), id(id), has_init(false), init(nullptr) {
    assert(type); assert(id);
}
VarDef::VarDef(Type* type, Id* id, Initializer* init)
    : type(type), id(id), has_init(true), init(init) {
    assert(type); assert(id); assert(init);
}

VolatileVarDef::VolatileVarDef(Type* type, Id* id): VarDef(type, id) {}
VolatileVarDef::VolatileVarDef(Type* type, Id* id, Initializer* init)
    : VarDef(type, id, init) {}

ConstVarDef::ConstVarDef(Type* type, Id* id): VarDef(type, id) {}
ConstVarDef::ConstVarDef(Type* type, Id* id, Initializer* init)
    : VarDef(type, id, init) {}

VarDecl::VarDecl(std::vector<VarDef*> var_defs) : var_defs(var_defs) {}

Return::Return(): return_type(NotHasRetVal) {}
Return::Return(Expr* ret_val): return_type(HasRetVal), ret_val(ret_val) {
    assert(ret_val);
}

Block::Block(std::vector<Stmt*> stmts): stmts(stmts) {}

If::If(Expr* cond, Stmt* then_stmt)
    : cond(cond), has_else_stmt(false), then_stmt(then_stmt) {
    assert(cond); assert(then_stmt);
}

If::If(Expr* cond, Stmt* then_stmt, Stmt* else_stmt)
    : cond(cond), has_else_stmt(true), then_stmt(then_stmt), else_stmt(else_stmt) {
    assert(cond); assert(then_stmt); assert(else_stmt);
}

While::While(Expr* cond, Stmt* body): cond(cond), body(body) {
    assert(cond); assert(body);
}

For::For (Stmt * init_stmt, Expr* cond, Stmt * iter_stmt, Stmt* body) 
    : init_stmt(init_stmt), cond(cond), iter_stmt(iter_stmt), body(body) {
    assert(init_stmt); assert(cond); assert(iter_stmt); assert(body);
}

GlobalVarDef::GlobalVarDef(Type* type, Id* id)
    : type(type), id(id), has_init(false), init(nullptr) {
    assert(type); assert(id);
}

GlobalVarDef::GlobalVarDef(Type* type, Id* id, Initializer* init) 
    : type(type), id(id), has_init(true), init(init) {
    assert(type); assert(id); assert(init);
}

VolatileGlobalVarDef::VolatileGlobalVarDef(Type* type, Id* id): GlobalVarDef(type, id) {}
VolatileGlobalVarDef::VolatileGlobalVarDef(Type* type, Id* id, Initializer* init)
    : GlobalVarDef(type, id, init) {}

ConstGlobalVarDef::ConstGlobalVarDef(Type* type, Id* id): GlobalVarDef(type, id) {}
ConstGlobalVarDef::ConstGlobalVarDef(Type* type, Id* id, Initializer* init)
    : GlobalVarDef(type, id, init) {}

GlobalVarDecl::GlobalVarDecl(std::vector<GlobalVarDef*> var_defs) : var_defs(var_defs) {}

FuncDef::FuncDef(
    Type* ret_type, Id* id, std::vector<std::tuple<Type *, Id*>*> params, Block* block
) : ret_type(ret_type), id(id), params(params), block(block) {
    assert(ret_type); assert(id); assert(block);
}

CompUnit::CompUnit(std::vector<ast::GlobalStmt*> global_stmts)
    : global_stmts(global_stmts) {}

}