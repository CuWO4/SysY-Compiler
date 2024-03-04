#include "../include/ast.h"

namespace ast {

BinaryExpr::BinaryExpr(op::BinaryOp op, Expr *lv, Expr* rv)
    : op(op), lv(lv), rv(rv) {}

UnaryExpr::UnaryExpr(op::UnaryOp op, Expr *lv)
    : op(op), lv(lv) {}

Id::Id(std::string *lit, NestingInfo *nesting_info)
    : lit(lit), nesting_info(nesting_info) {}

Number::Number(int val) : val(val) {}

VarDef::VarDef(Id *id) : id(id) {}
VarDef::VarDef(Id *id, Expr *init) : id(id), init(init) { has_init = true; }

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



FuncDef::FuncDef(Type *func_type, std::string *id, Block *block) :
    func_type(func_type), id(id), block(block) {}

CompUnit::CompUnit(FuncDef *func_def) : func_def(func_def) {}

}