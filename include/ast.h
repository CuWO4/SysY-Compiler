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
    };

        class Int: public Type {
        public:
            koopa::Type* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class Void :public Type {
            koopa::Type* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class Pointer: public Type {
        public:
            Pointer(Type* pointed_type);

            koopa::Type* to_koopa() const override;

            std::string debug(int indent = 0) const override;

        private:
            Type* pointed_type;
        };

        class Expr;
        class Array: public Type {
        public:
            Array(Type* element_type, Expr* length);

            koopa::Type* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        
        private:
            Type* element_type;
            Expr* length;
        };

    class Id {
    public:
        std::string lit;
        NestingInfo* nesting_info;

        Id(std::string lit, NestingInfo* nesting_info);

        std::string debug(int indent = 0) const;
    };

    class Stmt: public Base {
    public:
        virtual koopa_trans::Blocks* to_koopa() const = 0;
    };

        class Expr: public Stmt {
        public:
            /**
             * default implementation: has no side effect
             */
            virtual bool has_side_effect() const;

            /**
             * default implementation: not assignable, 
             * crash when assigning a not assignable Expr
             *
             * @example assignable: foo, arr[2][3]
             *          not assignable: 1 + 2
             */
            virtual bool is_assignable() const;
            virtual koopa_trans::Blocks* assign(const Expr* rv) const;
        };

            class BinaryExpr: public Expr {
            protected:
                BinaryExpr(Expr* lv, Expr* rv);

                Expr* lv;
                Expr* rv;

            public:
                virtual bool has_side_effect() const;
            };

                class LogicOr: public BinaryExpr {
                public:
                    LogicOr(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;
                    
                    std::string debug(int indent = 0) const override;
                };

                class LogicAnd: public BinaryExpr {
                public:
                    LogicAnd(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Eq: public BinaryExpr {
                public:
                    Eq(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Neq: public BinaryExpr {
                public:
                    Neq(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Lt: public BinaryExpr {
                public:
                    Lt(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Gt: public BinaryExpr {
                public:
                    Gt(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Leq: public BinaryExpr {
                public:
                    Leq(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Geq: public BinaryExpr {
                public:
                    Geq(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Add: public BinaryExpr {
                public:
                    Add(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Sub: public BinaryExpr {
                public:
                    Sub(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Mul: public BinaryExpr {
                public:
                    Mul(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Div: public BinaryExpr {
                public:
                    Div(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Mod: public BinaryExpr {
                public:
                    Mod(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Comma: public BinaryExpr {
                public:
                    Comma(Expr* lv, Expr* rv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Assign: public BinaryExpr {
                public:
                    Assign(Expr* lv, Expr* rv);

                    bool has_side_effect() const override;

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

            class UnaryExpr: public Expr {
            protected:
                UnaryExpr(Expr* lv);
                Expr* lv;

            public:
                bool has_side_effect() const override;
            };

                class Neg: public UnaryExpr {
                public:
                    Neg(Expr* lv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Pos: public UnaryExpr {
                public:
                    Pos(Expr* lv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

                class Not: public UnaryExpr {
                public:
                    Not(Expr* lv);

                    koopa_trans::Blocks* to_koopa() const override;

                    std::string debug(int indent = 0) const override;
                };

            class Id;
            class Indexing: public Expr {
            public:
                Indexing(Id* id, std::vector<Expr*> indexes);

                bool has_side_effect() const override;

                bool is_assignable() const override;
                koopa_trans::Blocks* assign(const Expr* rv) const override;

                koopa_trans::Blocks* to_koopa() const override;

                std::string debug(int indent = 0) const override;

            private:
                Id *id;
                std::vector<Expr*> indexes;

                std::tuple<koopa_trans::Blocks*, koopa::Id*> get_pointer(
                    koopa::Id* id
                ) const;
            };

            class FuncCall: public Expr {
            public:
                Id* func_id;
                std::vector<Expr*>  actual_params;

                FuncCall(Id* func_id, std::vector<Expr*> actual_params);

                koopa_trans::Blocks* to_koopa() const override;

                bool has_side_effect() const override;

                std::string debug(int indent = 0) const override;
            };

            class Number: public Expr {
            public:
                int val;

                Number(int val);

                koopa_trans::Blocks* to_koopa() const override;

                std::string debug(int indent = 0) const override;
            };

        class AggregateAgent;
        class Initializer: public Base {
        public:
            /**
             * @return dimension of initializer
             * @example 1.get_dim() => 0
             * @example { 1, {2, 3} }.get_dim() => 2
             */
            virtual unsigned get_dim() const = 0;
        
            virtual koopa::Initializer* initializer_to_koopa(
                std::vector<int> dimensions
            ) const = 0;

            /**
             * only work for `Initializer` with 0 dimension 
             * (in other word, `ConstInitializer`)
             * 
             * crash when calling expr_to_koopa() of an Initializer 
             * with non-zero dimensions
             */
            virtual koopa_trans::Blocks* expr_to_koopa() const = 0;

            virtual void initializer_to_koopa_agent(
                AggregateAgent& agent
            ) const = 0;
        };

            class ConstInitializer: public Initializer {
            public:
                ConstInitializer(Expr* val);

                unsigned get_dim() const override;

                /**
                 * val must be compile-time computable, otherwise
                 * @throw <std::string>
                 */
                koopa::Initializer* initializer_to_koopa(
                    std::vector<int> dimensions
                ) const override;

                void initializer_to_koopa_agent(
                    AggregateAgent& agent
                ) const override;

                koopa_trans::Blocks* expr_to_koopa() const override;

                std::string debug(int indent = 0) const override;
                
            private:
                Expr* val;
            };

            class Aggregate: public Initializer {
            public:
                Aggregate(std::vector<Initializer*> initializers);

                unsigned get_dim() const override;

                koopa::Initializer* initializer_to_koopa(
                    std::vector<int> dimensions
                ) const override;

                void initializer_to_koopa_agent(
                    AggregateAgent& agent
                ) const override;

                // crash
                koopa_trans::Blocks* expr_to_koopa() const override;

                std::string debug(int indent = 0) const override;
                
            private:
                std::vector<Initializer*> initializers;
            };

        class VarDef: public Stmt {
        protected:
            Type* type;
            Id* id;
            bool has_init;
            Initializer* init;

            VarDef(Type* type, Id* id);
            VarDef(Type* type, Id* id, Initializer* init);
        };

            class VolatileVarDef: public VarDef {
            public:
                VolatileVarDef(Type* type, Id* id);
                VolatileVarDef(Type* type, Id* id, Initializer* init);

                koopa_trans::Blocks* to_koopa() const override;

                std::string debug(int indent = 0) const override;
            };

            class ConstVarDef: public VarDef {
            public:
                ConstVarDef(Type* type, Id* id);
                ConstVarDef(Type* type, Id* id, Initializer* init);

                koopa_trans::Blocks* to_koopa() const override;

                std::string debug(int indent = 0) const override;
            };

        class VarDecl: public Stmt {
        public:
            std::vector<VarDef*> var_defs;

            VarDecl(std::vector<VarDef*>);

            koopa_trans::Blocks* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };


        class Return: public Stmt {
        public:
            enum ReturnType { HasRetVal, NotHasRetVal };
            
            Return();
            Return(Expr* ret_val);

            koopa_trans::Blocks* to_koopa() const override;

            std::string debug(int indent = 0) const override;

        private:
            ReturnType return_type;
            Expr* ret_val;
        };

        class Block: public Stmt {
        public:
            std::vector<Stmt*> stmts;

            Block(std::vector<Stmt*> stmts);

            koopa_trans::Blocks* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class If: public Stmt {
        public:
            Expr* cond;

            bool has_else_stmt;
            Stmt* then_stmt;
            Stmt* else_stmt;

            If(Expr* cond, Stmt* then_stmt);

            If(Expr* cond, Stmt* then_stmt, Stmt* else_stmt);

            koopa_trans::Blocks* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class While: public Stmt {
        public:
            Expr* cond;
            Stmt* body;

            While(Expr* cond, Stmt* body);

            koopa_trans::Blocks* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class For: public Stmt {
        public:
            Stmt* init_stmt;
            Expr* cond;
            Stmt* iter_stmt;
            Stmt* body;

            For (Stmt * init_stmt, Expr* cond, Stmt * iter_stmt, Stmt* body);

            koopa_trans::Blocks* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class Continue: public Stmt {
        public:
            koopa_trans::Blocks* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

        class Break: public Stmt {
        public:
            koopa_trans::Blocks* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

    class GlobalStmt: public Base {
    public:
        virtual koopa_trans::GlobalStmts* to_koopa() const = 0;
    };

        class FuncDef: public GlobalStmt {
        public:
            FuncDef(
                Type* ret_type, 
                Id* id, 
                std::vector<std::tuple<Type*, Id*>*> params, 
                Block* block
            );

            koopa_trans::GlobalStmts* to_koopa() const override;

            std::string debug(int indent = 0) const override;

        private:
            Type* ret_type;
            Id* id;
            std::vector<std::tuple<Type *, Id*>*> params;
            Block* block;
        };

        class FuncDecl: public GlobalStmt {
        public:
            FuncDecl(
                Type* ret_type, 
                Id* id, 
                std::vector<Type*> param_types
            );

            FuncDecl(
                Type* ret_type, 
                Id* id, 
                std::vector<std::tuple<Type*, Id*>*> params
            );

            koopa_trans::GlobalStmts* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        
        private:
            Type* ret_type;
            Id* id;
            std::vector<Type*> param_types;
        };

        class GlobalVarDef: public GlobalStmt {
        protected:
            Type* type;
            Id* id;
            bool has_init;
            Initializer* init;

            GlobalVarDef(Type* type, Id* id);

            GlobalVarDef(Type* type, Id* id, Initializer* init);
        };

            class VolatileGlobalVarDef: public GlobalVarDef {
            public:
                VolatileGlobalVarDef(Type* type, Id* id);
                VolatileGlobalVarDef(Type* type, Id* id, Initializer* init);

                koopa_trans::GlobalStmts* to_koopa() const override;

                std::string debug(int indent = 0) const override;
            };

            class ConstGlobalVarDef: public GlobalVarDef {
            public:
                ConstGlobalVarDef(Type* type, Id* id);
                ConstGlobalVarDef(Type* type, Id* id, Initializer* init);

                koopa_trans::GlobalStmts* to_koopa() const override;

                std::string debug(int indent = 0) const override;
            };

        class GlobalVarDecl: public GlobalStmt {
        public:
            std::vector<GlobalVarDef*> var_defs;

            GlobalVarDecl(std::vector<GlobalVarDef*> var_defs);

            koopa_trans::GlobalStmts* to_koopa() const override;

            std::string debug(int indent = 0) const override;
        };

    class CompUnit: public Base {
    public:
        std::vector<GlobalStmt*> global_stmts;

        CompUnit(std::vector<GlobalStmt*> global_stmts);

        koopa::Program* to_koopa() const;

        std::string debug(int indent = 0) const override;
    };

}

#endif