#ifndef AST_H_
#define AST_H_

#include "trans.h"
#include "koopa.h"
#include "nesting_info.h"

#include <iostream>
#include <vector>
#include <functional>

namespace koopa {
    class Base;
    class ValueSaver;
}

namespace ast {

class Base {
public:
    virtual std::string debug(int indent = 0) const = 0;

    virtual ~Base() = default;
};

class Type : public Base {
public:
    virtual koopa::Type* to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const = 0;
};

    class Int : public Type {
    public:
        koopa::Type* to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const override;

        std::string debug(int indent = 0) const override;
    };

namespace op {
    enum BinaryOp {
        LOGIC_OR, LOGIC_AND, EQ, NEQ, LT, GT, LEQ, GEQ,
        ADD, SUB, MUL, DIV, MOD,
    };

    enum UnaryOp {
        NEG, POS, NOT,
    };
}

class Expr : public Base {
public:
    virtual koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const = 0;
};

    class BinaryExpr : public Expr {
    public:
        op::BinaryOp    op  = op::LOGIC_OR;
        Expr            *lv = nullptr;
        Expr            *rv = nullptr;

        BinaryExpr(op::BinaryOp op, Expr *lv, Expr* rv) :
            op(op), lv(lv), rv(rv) {}

        koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const override;

        std::string debug(int indent = 0) const override;

        ~BinaryExpr() override;
    };

    class UnaryExpr : public Expr {
    public:
        op::UnaryOp     op  = op::NEG;
        Expr            *lv = nullptr;

        UnaryExpr(op::UnaryOp op, Expr *lv) :
            op(op), lv(lv) {}

        koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const override;

        std::string debug(int indent = 0) const override;

        ~UnaryExpr() override;
    };

    class Id : public Expr {
    public:
        std::string *lit;

        Id(std::string *lit) : lit(lit) {}

        koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const override;

        std::string debug(int indent = 0) const override;

        ~Id() override;
    };

    class Number : public Expr {
    public:
        int val = 0;

        Number(int val) : val(val) {}

        koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const override;

        std::string debug(int indent = 0) const override;
    };

class Stmt : public Base {
public:
    virtual koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const = 0;
};

    class ExprStmt : public Stmt {
    public:
        Expr *expr = nullptr;;

        ExprStmt(Expr *expr) : expr(expr) {}

        koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const override;

        std::string debug(int indent = 0) const override;

        ~ExprStmt() override;        
    };

    class VarDef : public Stmt {
    public:
        Id *id = nullptr;
        bool has_init = false;
        Expr *init = nullptr;

        VarDef(Id *id) : id(id) {}
        VarDef(Id *id, Expr *init) : id(id), init(init) { has_init = true; }

        koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const override { return nullptr; }

        std::string debug(int indent = 0) const override;

        ~VarDef();
    };
    class VarDecl : public Stmt {
    public:
        Type *type = nullptr;
        std::vector<VarDef *> var_defs = {};
        bool is_const = false;

        VarDecl(Type *type, std::vector<VarDef *> var_defs, 
                bool is_const = false) :
            type(type), var_defs(var_defs), is_const(is_const) {}

        koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const override;

        std::string debug(int indent = 0) const override;

        ~VarDecl() override;
    };

    class Assign : public Stmt {
    public:
        Id *id = nullptr;
        Expr *rval = nullptr;

        Assign(Id *id, Expr *rval) : id(id), rval(rval) {}

        koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const override;

        std::string debug(int indent = 0) const override;

        ~Assign() override;
    };

    class Return : public Stmt {
    public:
        bool has_return_val = true;
        Expr *ret_val = nullptr;

        Return() : has_return_val(false) {}
        Return(Expr *ret_val) : ret_val(ret_val) {}

        koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const override;

        std::string debug(int indent = 0) const override;

        ~Return() override;
    };

    class Block : public Stmt {
    public:
        std::vector<Stmt *> stmts = {};

        NestingInfo *nesting_info;

        Block(std::vector<Stmt *> stmts, NestingInfo *nesting_info) 
            : stmts(stmts), nesting_info(nesting_info) {}

        koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const override;

        std::string debug(int indent = 0) const override;

        ~Block() override;
    };

    class If : public Stmt {
    public:
        Expr *cond = nullptr;

        bool has_else_stmt = false;
        Stmt *then_stmt = nullptr;
        Stmt *else_stmt = nullptr;

        If(Expr *cond, Stmt *then_stmt)
            : cond(cond), has_else_stmt(false), then_stmt(then_stmt) {}

        If(Expr *cond, Stmt *then_stmt, Stmt *else_stmt)
            : cond(cond), has_else_stmt(true), then_stmt(then_stmt), else_stmt(else_stmt) {}

        koopa_trans::Blocks *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const override;

        std::string debug(int indent = 0) const override;

        ~If() override;
    };

class FuncDef : public Base {
public:
    Type            *func_type  = nullptr;
    std::string     *id         = nullptr;
    Block           *block      = nullptr;

    FuncDef(Type *func_type, std::string *id, Block *block) :
        func_type(func_type), id(id), block(block) {}

    koopa::FuncDef *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const;

    std::string debug(int indent = 0) const override;

    ~FuncDef() override;
};

class CompUnit : public Base {
public:
    FuncDef *func_def = nullptr;

    CompUnit(FuncDef *func_def) : func_def(func_def) {}

    koopa::Program *to_koopa(koopa::ValueSaver &value_saver, NestingInfo *nesting_info) const;

    std::string debug(int indent = 0) const override;

    ~CompUnit() override;
};

}

#endif