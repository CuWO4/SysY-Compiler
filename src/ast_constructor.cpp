#include "../include/ast.h"

namespace ast {

BinaryExpr::BinaryExpr(op::BinaryOp op, Expr *lv, Expr* rv)
    : op(op), lv(lv), rv(rv) {}

UnaryExpr::UnaryExpr(op::UnaryOp op, Expr *lv)
    : op(op), lv(lv) {}

Id::Id(std::string *lit, NestingInfo *nesting_info)
    : lit(lit), nesting_info(nesting_info) {}

FuncCall::FuncCall(Id *func_id, std::vector<Expr *> actual_params) 
    : func_id(func_id), actual_params(actual_params) {}

Number::Number(int val) : val(val) {}

VarDef::VarDef(Id *id) : id(id) {}
VarDef::VarDef(Id *id, Expr *init) : id(id), has_init(true), init(init) {}

VarDecl::VarDecl(Type *type, std::vector<VarDef *> var_defs, bool is_const)
    : type(type), var_defs(var_defs), is_const(is_const) {}

Return::Return() : has_return_val(false) {}
Return::Return(Expr *ret_val) : ret_val(ret_val) {}

Block::Block(std::vector<Stmt *> stmts) : stmts(stmts) {}

If::If(Expr *cond, Stmt *then_stmt)
    : cond(cond), has_else_stmt(false), then_stmt(then_stmt) {}

If::If(Expr *cond, Stmt *then_stmt, Stmt *else_stmt)
    : cond(cond), has_else_stmt(true), then_stmt(then_stmt), else_stmt(else_stmt) {}

While::While(Expr *cond, Stmt *body) : cond(cond), body(body) {}

For::For (Stmt * init_stmt, Expr *cond, Stmt * iter_stmt, Stmt *body) 
    : init_stmt(init_stmt), cond(cond), iter_stmt(iter_stmt), body(body) {}

GlobalVarDef::GlobalVarDef(Id *id) : id(id), has_init(false) {}
GlobalVarDef::GlobalVarDef(Id *id, Expr *init) : id(id), has_init(true), init(init) {}

GlobalVarDecl::GlobalVarDecl(Type *type, std::vector<GlobalVarDef *> var_defs, bool is_const)
    : type(type), var_defs(var_defs), is_const(is_const) {}

FuncDef::FuncDef(Type *ret_type, Id *id, std::vector<std::tuple<Type *, Id *> *> params, Block *block) :
    ret_type(ret_type), id(id), params(params), block(block) {}

CompUnit::CompUnit(std::vector<ast::GlobalStmt *> global_stmts) : global_stmts(global_stmts) {}

}