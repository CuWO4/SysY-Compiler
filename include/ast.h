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
    virtual koopa::Type* to_koopa() const = 0;
};

    class Int : public Type {
    public:
        koopa::Type* to_koopa() const override;

        std::string debug(int indent = 0) const override;
    };

    class Void :public Type {
        koopa::Type *to_koopa() const override;

        std::string debug(int indent = 0) const override;
    };

namespace op {
    enum BinaryOp {
        LOGIC_OR, LOGIC_AND, EQ, NEQ, LT, GT, LEQ, GEQ,
        ADD, SUB, MUL, DIV, MOD, COMMA, ASSIGN
    };

    enum UnaryOp {
        NEG, POS, NOT,
    };
}

class Stmt : public Base {
public:
    virtual koopa_trans::Blocks *to_koopa() const = 0;
};

    class Expr : public Stmt {
    public:
        virtual bool has_side_effect() const = 0;
    };

        class BinaryExpr : public Expr {
        public:
            op::BinaryOp    op  = op::LOGIC_OR;
            Expr            *lv = nullptr;
            Expr            *rv = nullptr;

            BinaryExpr(op::BinaryOp op, Expr *lv, Expr* rv);

            koopa_trans::Blocks *to_koopa() const override;

            bool has_side_effect() const override;

            std::string debug(int indent = 0) const override;

            ~BinaryExpr() override;
        };

        class UnaryExpr : public Expr {
        public:
            op::UnaryOp     op  = op::NEG;
            Expr            *lv = nullptr;

            UnaryExpr(op::UnaryOp op, Expr *lv);

            koopa_trans::Blocks *to_koopa() const override;

            bool has_side_effect() const override;

            std::string debug(int indent = 0) const override;

            ~UnaryExpr() override;
        };

        class Id : public Expr {
        public:
            std::string *lit;
            NestingInfo *nesting_info = nullptr;

            Id(std::string *lit, NestingInfo *nesting_info);

            koopa_trans::Blocks *to_koopa() const override;

            bool has_side_effect() const override;

            std::string debug(int indent = 0) const override;

            ~Id() override;
        };

        class FuncCall : public Expr {
        public:
            Id *func_id = nullptr;
            std::vector<Expr *> actual_params = {};

            FuncCall(Id *func_id, std::vector<Expr *> actual_params);

            koopa_trans::Blocks *to_koopa() const override;

            bool has_side_effect() const override;

            std::string debug(int indent = 0) const override;

            ~FuncCall() override;
        };

        class Number : public Expr {
        public:
            int val = 0;

            Number(int val);

            koopa_trans::Blocks *to_koopa() const override;

            bool has_side_effect() const override;

            std::string debug(int indent = 0) const override;
        };

    class VarDef : public Stmt {
    public:
        Id *id = nullptr;
        bool has_init = false;
        Expr *init = nullptr;

        VarDef(Id *id);
        VarDef(Id *id, Expr *init);

        koopa_trans::Blocks *to_koopa() const override { return nullptr; }

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

        koopa_trans::Blocks *to_koopa() const override;

        std::string debug(int indent = 0) const override;

        ~VarDecl() override;
    };

    class Return : public Stmt {
    public:
        bool has_return_val = true;
        Expr *ret_val = nullptr;

        Return();
        Return(Expr *ret_val);

        koopa_trans::Blocks *to_koopa() const override;

        std::string debug(int indent = 0) const override;

        ~Return() override;
    };

    class Block : public Stmt {
    public:
        std::vector<Stmt *> stmts = {};

        Block(std::vector<Stmt *> stmts);

        koopa_trans::Blocks *to_koopa() const override;

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

        koopa_trans::Blocks *to_koopa() const override;

        std::string debug(int indent = 0) const override;

        ~If() override;
    };

    class While : public Stmt {
    public:
        Expr *cond =nullptr;
        Stmt *body = nullptr;

        While(Expr *cond, Stmt *body);

        koopa_trans::Blocks *to_koopa() const override;

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

        koopa_trans::Blocks *to_koopa() const override;

        std::string debug(int indent = 0) const override;

        ~For() override;
    };

    class Continue : public Stmt {
    public:
        koopa_trans::Blocks *to_koopa() const override;

        std::string debug(int indent = 0) const override;
    };

    class Break : public Stmt {
    public:
        koopa_trans::Blocks *to_koopa() const override;

        std::string debug(int indent = 0) const override;
    };

class GlobalStmt : public Base {
public:
    virtual koopa::GlobalStmt *to_koopa() const = 0;
};

    class FuncDef : public GlobalStmt {
    public:
        Type            *ret_type  = nullptr;
        Id              *id         = nullptr;
        std::vector<std::tuple<Type *, Id *> *> params = {};
        Block           *block      = nullptr;

        FuncDef(Type *ret_type, Id *id, std::vector<std::tuple<Type *, Id *> *> params, Block *block);

        koopa::GlobalStmt *to_koopa() const override;

        std::string debug(int indent = 0) const override;

        ~FuncDef() override;
    };

class CompUnit : public Base {
public:
    std::vector<GlobalStmt *> global_stmts = {};

    CompUnit(std::vector<GlobalStmt *> global_stmts);

    koopa::Program *to_koopa() const;

    std::string debug(int indent = 0) const override;

    ~CompUnit() override;
};

}

#endif