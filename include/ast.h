#ifndef AST_H_
#define AST_H_

#include "trans.h"
#include "koopa.h"
#include "value_saver.h"
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
    virtual koopa::Type* to_koopa(ValueSaver &value_saver) const = 0;
};

    class Int : public Type {
    public:
        koopa::Type* to_koopa(ValueSaver &value_saver) const override;

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
    virtual koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const = 0;

    virtual bool has_side_effect() const = 0;
};

    class BinaryExpr : public Expr {
    public:
        op::BinaryOp    op  = op::LOGIC_OR;
        Expr            *lv = nullptr;
        Expr            *rv = nullptr;

        BinaryExpr(op::BinaryOp op, Expr *lv, Expr* rv);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        bool has_side_effect() const override;

        std::string debug(int indent = 0) const override;

        ~BinaryExpr() override;
    };

    class UnaryExpr : public Expr {
    public:
        op::UnaryOp     op  = op::NEG;
        Expr            *lv = nullptr;

        UnaryExpr(op::UnaryOp op, Expr *lv);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        bool has_side_effect() const override;

        std::string debug(int indent = 0) const override;

        ~UnaryExpr() override;
    };

    class Id : public Expr {
    public:
        std::string *lit;
        NestingInfo *nesting_info = nullptr;

        Id(std::string *lit, NestingInfo *nesting_info);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        bool has_side_effect() const override;

        std::string debug(int indent = 0) const override;

        ~Id() override;
    };

    class Number : public Expr {
    public:
        int val = 0;

        Number(int val);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        bool has_side_effect() const override;

        std::string debug(int indent = 0) const override;
    };

class Stmt : public Base {
public:
    virtual koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const = 0;
};

    class ExprStmt : public Stmt {
    public:
        Expr *expr = nullptr;;

        ExprStmt(Expr *expr);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        std::string debug(int indent = 0) const override;

        ~ExprStmt() override;        
    };

    class VarDef : public Stmt {
    public:
        Id *id = nullptr;
        bool has_init = false;
        Expr *init = nullptr;

        VarDef(Id *id);
        VarDef(Id *id, Expr *init);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override { return nullptr; }

        std::string debug(int indent = 0) const override;

        ~VarDef();
    };
    class VarDecl : public Stmt {
    public:
        Type *type = nullptr;
        std::vector<VarDef *> var_defs = {};
        bool is_const = false;

        VarDecl(Type *type, std::vector<VarDef *> var_defs, 
                bool is_const = false);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        std::string debug(int indent = 0) const override;

        ~VarDecl() override;
    };

    class Assign : public Stmt {
    public:
        Id *id = nullptr;
        Expr *rval = nullptr;

        Assign(Id *id, Expr *rval);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        std::string debug(int indent = 0) const override;

        ~Assign() override;
    };

    class Return : public Stmt {
    public:
        bool has_return_val = true;
        Expr *ret_val = nullptr;

        Return();
        Return(Expr *ret_val);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        std::string debug(int indent = 0) const override;

        ~Return() override;
    };

    class Block : public Stmt {
    public:
        std::vector<Stmt *> stmts = {};

        Block(std::vector<Stmt *> stmts);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        std::string debug(int indent = 0) const override;

        ~Block() override;
    };

    class If : public Stmt {
    public:
        Expr *cond = nullptr;

        bool has_else_stmt = false;
        Stmt *then_stmt = nullptr;
        Stmt *else_stmt = nullptr;

        If(Expr *cond, Stmt *then_stmt);

        If(Expr *cond, Stmt *then_stmt, Stmt *else_stmt);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        std::string debug(int indent = 0) const override;

        ~If() override;
    };

    class While : public Stmt {
    public:
        Expr *cond =nullptr;
        Stmt *body = nullptr;

        While(Expr *cond, Stmt *body);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        std::string debug(int indent = 0) const override;

        ~While() override;
    };

    class For : public Stmt {
    public:
        Stmt *init_stmt = {};
        Expr *cond =nullptr;
        Stmt * iter_stmt = {};
        Stmt *body = nullptr;

        For (Stmt * init_stmt, Expr *cond, Stmt * iter_stmt, Stmt *body);

        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        std::string debug(int indent = 0) const override;

        ~For() override;
    };

    class Continue : public Stmt {
    public:
        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        std::string debug(int indent = 0) const override;
    };

    class Break : public Stmt {
    public:
        koopa_trans::Blocks *to_koopa(ValueSaver &value_saver) const override;

        std::string debug(int indent = 0) const override;
    };

class FuncDef : public Base {
public:
    Type            *func_type  = nullptr;
    std::string     *id         = nullptr;  //TODO it's even not id
    Block           *block      = nullptr;

    FuncDef(Type *func_type, std::string *id, Block *block);

    koopa::FuncDef *to_koopa(ValueSaver &value_saver) const;

    std::string debug(int indent = 0) const override;

    ~FuncDef() override;
};

class CompUnit : public Base {
public:
    FuncDef *func_def = nullptr;

    CompUnit(FuncDef *func_def);

    koopa::Program *to_koopa(ValueSaver &value_saver) const;

    std::string debug(int indent = 0) const override;

    ~CompUnit() override;
};

}

#endif