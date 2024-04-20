#ifndef AST_H_
#define AST_H_

#include "koopa_trans.h"
#include "koopa.h"
#include "value_manager.h"
#include "nesting_info.h"

#include <vector>

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
        public:
            Pointer(Type *pointed_type);

            koopa::Type *to_koopa() const override;

            std::string debug(int indent = 0) const override;

        private:
            Type *pointed_type;
        };

        class Expr;
        class Array: public Type {
        public:
            Array(Type *element_type, Expr *length);

            koopa::Type *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        
        private:
            Type *element_type;
            Expr *length;
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
                BinaryOp    op;
                Expr       *lv;
                Expr       *rv;

                BinaryExpr(BinaryOp op, Expr *lv, Expr* rv);

                koopa_trans::Blocks *to_koopa() const override;

                bool has_side_effect() const override;

                std::string debug(int indent = 0) const override;
            };

            class UnaryExpr: public Expr {
            public:
                UnaryOp     op;
                Expr       *lv;

                UnaryExpr(UnaryOp op, Expr *lv);

                koopa_trans::Blocks *to_koopa() const override;

                bool has_side_effect() const override;

                std::string debug(int indent = 0) const override;
            };

            class Id: public Expr {
            public:
                std::string     lit;
                NestingInfo    *nesting_info;

                Id(std::string lit, NestingInfo *nesting_info);

                koopa_trans::Blocks *to_koopa() const override;

                bool has_side_effect() const override;

                std::string debug(int indent = 0) const override;
            };

            class FuncCall: public Expr {
            public:
                Id                  *func_id;
                std::vector<Expr *>  actual_params;

                FuncCall(Id *func_id, std::vector<Expr *> actual_params);

                koopa_trans::Blocks *to_koopa() const override;

                bool has_side_effect() const override;

                std::string debug(int indent = 0) const override;
            };

            class Number: public Expr {
            public:
                int val;

                Number(int val);

                koopa_trans::Blocks *to_koopa() const override;

                bool has_side_effect() const override;

                std::string debug(int indent = 0) const override;
            };

        namespace decl_type {
            enum DeclType { VolatileDecl, ConstDecl };
        }
        using DeclType = decl_type::DeclType;

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
            std::vector<VarDef *> var_defs;

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
            ReturnType  return_type;
            Expr       *ret_val;

            Return();
            Return(Expr *ret_val);

            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class Block: public Stmt {
        public:
            std::vector<Stmt *> stmts;

            Block(std::vector<Stmt *> stmts);

            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class If: public Stmt {
        public:
            Expr *cond;

            bool  has_else_stmt;
            Stmt *then_stmt;
            Stmt *else_stmt;

            If(Expr *cond, Stmt *then_stmt);

            If(Expr *cond, Stmt *then_stmt, Stmt *else_stmt);

            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class While: public Stmt {
        public:
            Expr *cond;
            Stmt *body;

            While(Expr *cond, Stmt *body);

            koopa_trans::Blocks *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class For: public Stmt {
        public:
            Stmt *init_stmt;
            Expr *cond;
            Stmt *iter_stmt;
            Stmt *body;

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
            Type            *ret_type;
            Id              *id;
            std::vector<std::tuple<Type *, Id *> *> params;
            Block           *block;

            FuncDef(
                Type *ret_type, 
                Id *id, 
                std::vector<std::tuple<Type *, Id *> *> params, 
                Block *block
            );

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
            std::vector<GlobalVarDef *> var_defs;

            GlobalVarDecl(std::vector<GlobalVarDef *> var_defs);

            koopa_trans::GlobalStmts *to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

    class CompUnit: public Base {
    public:
        std::vector<GlobalStmt *> global_stmts;

        CompUnit(std::vector<GlobalStmt *> global_stmts);

        koopa::Program *to_koopa() const;

        std::string debug(int indent = 0) const override;
    };

}

#endif