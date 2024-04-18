#ifndef AST_H_
#define AST_H_

#include "koopa_trans.h"
#include "koopa.h"
#include "value_manager.h"
#include "nesting_info.h"

#include <iostream>
#include <vector>
#include <functional>

namespace koopa {
    class Base;
    class ValueManager;
}

namespace ast {

    class Base {
    public:
        virtual std::string debug(int indent = 0) const = 0;

        virtual ~Base() = default;
    };

    class Type: public Base {
    public:
        virtual koopa::Type* to_koopa() const = 0;

        // TODO
        // virtual bool operator==(Type &other) = 0;
    };

        class Int: public Type {
        public:
            koopa::Type* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class Void :public Type {
            koopa::Type *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class Pointer: public Type {
            // TODO
        };

        class Array: public Type {
            // TODO
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
    using BinaryOp = op::BinaryOp;
    using UnaryOp = op::UnaryOp;

    class Stmt: public Base {
    public:
        virtual koopa_trans::Blocks *to_koopa() const = 0;
    };

        class Expr: public Stmt {
        public:
            virtual bool has_side_effect() const = 0;
        };

            class BinaryExpr: public Expr {
            public:
                BinaryOp        op  = op::LOGIC_OR;
                Expr           *lv = nullptr;
                Expr           *rv = nullptr;

                BinaryExpr(BinaryOp op, Expr *lv, Expr* rv);

                koopa_trans::Blocks *to_koopa() const override;

                bool has_side_effect() const override;

                std::string debug(int indent = 0) const override;
            };

            class UnaryExpr: public Expr {
            public:
                UnaryOp     op  = op::NEG;
                Expr            *lv = nullptr;

                UnaryExpr(UnaryOp op, Expr *lv);

                koopa_trans::Blocks *to_koopa() const override;

                bool has_side_effect() const override;

                std::string debug(int indent = 0) const override;
            };

            class Id: public Expr {
            public:
                std::string *lit;
                NestingInfo *nesting_info = nullptr;

                Id(std::string *lit, NestingInfo *nesting_info);

                koopa_trans::Blocks *to_koopa() const override;

                bool has_side_effect() const override;

                std::string debug(int indent = 0) const override;
            };

            class FuncCall: public Expr {
            public:
                Id *func_id = nullptr;
                std::vector<Expr *> actual_params = {};

                FuncCall(Id *func_id, std::vector<Expr *> actual_params);

                koopa_trans::Blocks *to_koopa() const override;

                bool has_side_effect() const override;

                std::string debug(int indent = 0) const override;
            };

            class Number: public Expr {
            public:
                int val = 0;

                Number(int val);

                koopa_trans::Blocks *to_koopa() const override;

                bool has_side_effect() const override;

                std::string debug(int indent = 0) const override;
            };

        namespace decl_type {
            enum DeclType { VolatileDecl, ConstDecl };
        }
        using DeclType = decl_type::DeclType;

        // TODO  change to template class, which represent it's a local/global VarDef
        class VarDef: public Stmt {
        public:
            DeclType    decl_type;
            Type       *type;
            Id         *id;
            bool        has_init;
            Expr       *init;

            VarDef(Type *, Id *, DeclType = decl_type::VolatileDecl);
            VarDef(
                Type *, Id *, Expr *, 
                DeclType = decl_type::VolatileDecl
            );

            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class VarDecl: public Stmt {
        public:
            std::vector<VarDef *> var_defs = {};

            VarDecl(std::vector<VarDef *>);

            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        namespace return_type {
            enum ReturnType { HasRetVal, NotHasRetVal };
        }
        using ReturnType = return_type::ReturnType;

        class Return: public Stmt {
        public:
            ReturnType return_type = return_type::NotHasRetVal;
            Expr *ret_val = nullptr;

            Return();
            Return(Expr *ret_val);

            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class Block: public Stmt {
        public:
            std::vector<Stmt *> stmts = {};

            Block(std::vector<Stmt *> stmts);

            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class If: public Stmt {
        public:
            Expr *cond = nullptr;

            bool has_else_stmt = false;
            Stmt *then_stmt = nullptr;
            Stmt *else_stmt = nullptr;

            If(Expr *cond, Stmt *then_stmt);

            If(Expr *cond, Stmt *then_stmt, Stmt *else_stmt);

            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class While: public Stmt {
        public:
            Expr *cond =nullptr;
            Stmt *body = nullptr;

            While(Expr *cond, Stmt *body);

            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class For: public Stmt {
        public:
            Stmt *init_stmt = {};
            Expr *cond =nullptr;
            Stmt * iter_stmt = {};
            Stmt *body = nullptr;

            For (Stmt * init_stmt, Expr *cond, Stmt * iter_stmt, Stmt *body);

            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class Continue: public Stmt {
        public:
            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class Break: public Stmt {
        public:
            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

    class GlobalStmt: public Base {
    public:
        virtual koopa_trans::GlobalStmts *to_koopa() const = 0;
    };

        class FuncDef: public GlobalStmt {
        public:
            Type            *ret_type  = nullptr;
            Id              *id         = nullptr;
            std::vector<std::tuple<Type *, Id *> *> params = {};
            Block           *block      = nullptr;

            FuncDef(Type *ret_type, Id *id, std::vector<std::tuple<Type *, Id *> *> params, Block *block);

            koopa_trans::GlobalStmts *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        // TODO
        class GlobalVarDef: public GlobalStmt {
        public:
            DeclType    decl_type;
            Type       *type;
            Id         *id;
            bool        has_init;
            Expr       *init;

            GlobalVarDef(Type *, Id *, DeclType = decl_type::VolatileDecl);
            GlobalVarDef(
                Type *, Id *, Expr *, 
                DeclType = decl_type::VolatileDecl
            );
            koopa_trans::GlobalStmts *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };


        // TODO
        class GlobalVarDecl: public GlobalStmt {
        public:
            std::vector<GlobalVarDef *> var_defs = {};

            GlobalVarDecl(std::vector<GlobalVarDef *> var_defs);

            koopa_trans::GlobalStmts *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

    class CompUnit: public Base {
    public:
        std::vector<GlobalStmt *> global_stmts = {};

        CompUnit(std::vector<GlobalStmt *> global_stmts);

        koopa::Program *to_koopa() const;

        std::string debug(int indent = 0) const override;
    };

}

#endif