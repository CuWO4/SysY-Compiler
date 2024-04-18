#include "../include/ast.h"

namespace ast {

BinaryExpr::BinaryExpr(BinaryOp op, Expr *lv, Expr* rv)
    : op(op), lv(lv), rv(rv) {}

UnaryExpr::UnaryExpr(UnaryOp op, Expr *lv)
    : op(op), lv(lv) {}

Id::Id(std::string *lit, NestingInfo *nesting_info)
    : lit(lit), nesting_info(nesting_info) {}

FuncCall::FuncCall(Id *func_id, std::vector<Expr *> actual_params) 
    : func_id(func_id), actual_params(actual_params) {}

Number::Number(int val): val(val) {}

VarDef::VarDef(Type *type, Id *id, DeclType decl_type)
    : decl_type(decl_type), type(type), id(id), has_init(false), init(nullptr) {}
VarDef::VarDef(
    Type *type, Id *id, Expr *init, DeclType decl_type
) : decl_type(decl_type), type(type), id(id), has_init(true), init(init) {}

VarDecl::VarDecl(std::vector<VarDef *> var_defs) : var_defs(var_defs) {}

Return::Return(): return_type(return_type::NotHasRetVal) {}
Return::Return(Expr *ret_val): return_type(return_type::HasRetVal), ret_val(ret_val) {}

Block::Block(std::vector<Stmt *> stmts): stmts(stmts) {}

If::If(Expr *cond, Stmt *then_stmt)
    : cond(cond), has_else_stmt(false), then_stmt(then_stmt) {}

If::If(Expr *cond, Stmt *then_stmt, Stmt *else_stmt)
    : cond(cond), has_else_stmt(true), then_stmt(then_stmt), else_stmt(else_stmt) {}

While::While(Expr *cond, Stmt *body): cond(cond), body(body) {}

For::For (Stmt * init_stmt, Expr *cond, Stmt * iter_stmt, Stmt *body) 
    : init_stmt(init_stmt), cond(cond), iter_stmt(iter_stmt), body(body) {}

GlobalVarDef::GlobalVarDef(Type *type, Id *id, DeclType decl_type)
    : decl_type(decl_type), type(type), id(id), has_init(false), init(nullptr) {}
GlobalVarDef::GlobalVarDef(
    Type *type, Id *id, Expr *init, DeclType decl_type
) : decl_type(decl_type), type(type), id(id), has_init(true), init(init) {}

GlobalVarDecl::GlobalVarDecl(std::vector<GlobalVarDef *> var_defs) : var_defs(var_defs) {}

FuncDef::FuncDef(Type *ret_type, Id *id, std::vector<std::tuple<Type *, Id *> *> params, Block *block) :
    ret_type(ret_type), id(id), params(params), block(block) {}

CompUnit::CompUnit(std::vector<ast::GlobalStmt *> global_stmts): global_stmts(global_stmts) {}

}