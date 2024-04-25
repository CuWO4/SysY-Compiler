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

        /**
         * @return type dimension
         * @example int => 0 
         * @example int[][2].get_dim() const => 2 
         */
        virtual int get_dim() const = 0;
    };

        class Int: public Type {
        public:
            koopa::Type* to_koopa() const override;

            std::string debug(int indent = 0) const override;

            int get_dim() const override;
        };

        class Void :public Type {
            koopa::Type *to_koopa() const override;

            std::string debug(int indent = 0) const override;

            int get_dim() const override;
        };

        class Pointer: public Type {
        public:
            Pointer(Type *pointed_type);

            koopa::Type *to_koopa() const override;

            std::string debug(int indent = 0) const override;

            int get_dim() const override;

        private:
            Type *pointed_type;
        };

        class Expr;
        class Array: public Type {
        public:
            Array(Type *element_type, Expr *length);

            koopa::Type *to_koopa() const override;

            std::string debug(int indent = 0) const override;

            int get_dim() const override;
        
        private:
            Type *element_type;
            Expr *length;
        };

    class Stmt: public Base {
    public:
        virtual koopa_trans::Blocks *to_koopa() const = 0;
    };

        class Expr: public Stmt {
        public:
            virtual bool has_side_effect() const = 0;
        };

            class BinaryExpr: public Expr {
            protected:
                BinaryExpr(Expr *lv, Expr *rv);

                Expr *lv;
                Expr *rv;

            public:
                bool has_side_effect() const;
            };

                class LogicOr: public BinaryExpr {
                public:
                    LogicOr(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;
                    
                    std::string debug(int indent = 0) const override;
                };

                class LogicAnd: public BinaryExpr {
                public:
                    LogicAnd(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Eq: public BinaryExpr {
                public:
                    Eq(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Neq: public BinaryExpr {
                public:
                    Neq(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Lt: public BinaryExpr {
                public:
                    Lt(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Gt: public BinaryExpr {
                public:
                    Gt(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Leq: public BinaryExpr {
                public:
                    Leq(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Geq: public BinaryExpr {
                public:
                    Geq(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Add: public BinaryExpr {
                public:
                    Add(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Sub: public BinaryExpr {
                public:
                    Sub(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Mul: public BinaryExpr {
                public:
                    Mul(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Div: public BinaryExpr {
                public:
                    Div(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Mod: public BinaryExpr {
                public:
                    Mod(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Comma: public BinaryExpr {
                public:
                    Comma(Expr *lv, Expr *rv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Assign: public BinaryExpr {
                public:
                    Assign(Expr *lv, Expr *rv);

                    bool has_side_effect() const override;

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

            class UnaryExpr: public Expr {
            protected:
                UnaryExpr(Expr *lv);
                Expr *lv;

            public:
                bool has_side_effect() const override;
            };

                class Neg: public UnaryExpr {
                public:
                    Neg(Expr *lv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Pos: public UnaryExpr {
                public:
                    Pos(Expr *lv);

                    koopa_trans::Blocks *to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Not: public UnaryExpr {
                public:
                    Not(Expr *lv);

                    koopa_trans::Blocks *to_koopa() const override;

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

        class Initializer: public Base {
        public:
            /**
             * @return dimension of initializer
             * @example 1.get_dim() => 0
             * @example { 1, {2, 3} }.get_dim() => 2
             */
            virtual int get_dim() const = 0;
        
            virtual koopa::Initializer *initializer_to_koopa() const = 0;

            /**
             * only work for `Initializer` with 0 dimension 
             * (in other word, `ConstInitializer`)
             * 
             * crash when calling expr_to_koopa() of an Initializer 
             * with non-zero dimensions
             */
            virtual koopa_trans::Blocks *expr_to_koopa() const = 0;
        };

            class ConstInitializer: public Initializer {
            public:
                ConstInitializer(Expr *val);

                int get_dim() const override;

                /**
                 * val must be compile-time computable, otherwise
                 * @throw <std::string>
                 */
                koopa::Initializer *initializer_to_koopa() const override;

                koopa_trans::Blocks *expr_to_koopa() const override;
                
            private:
                Expr *val;
            };

            class Aggregate: public Initializer {
            public:
                Aggregate(std::vector<Initializer *> initializers);

                int get_dim() const override;

                // align to boundaries
                koopa::Initializer *initializer_to_koopa() const override;

                // crash
                koopa_trans::Blocks *expr_to_koopa() const override;
                
            private:
                std::vector<Initializer *> initializers;
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