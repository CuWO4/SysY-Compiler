#ifndef KOOPA_H_
#define KOOPA_H_

#include "nesting_info.h"
#include "riscv_trans.h"

#include <assert.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace koopa {

class Base {
public:
    virtual std::string to_string() const = 0;

    virtual ~Base() = default;
};

namespace type {
    enum TypeId { Int, Array, Pointer, FuncType, Void, Label };
}
using TypeId = type::TypeId;

class Type: public Base {
public:
    virtual TypeId get_type_id() = 0;
    virtual bool operator==(Type &other) = 0;
    bool operator!=(Type &other);

    virtual unsigned get_byte_size() const;
};

    class Int: public Type {

        std::string to_string() const override;

        TypeId get_type_id() override;
        bool operator==(Type &other) override;

        unsigned get_byte_size() const override;
    };

    class Array: public Type {
    public:
        Type   *elem_type;
        int     length;

        std::string to_string() const override;

        Array(Type *elem_type, int length);

        TypeId get_type_id() override;
        bool operator==(Type &other) override;

        unsigned get_byte_size() const override;
    };

    class Pointer: public Type {
    public:
        Type *pointed_type;

        std::string to_string() const override;

        Pointer(Type *pointed_type);

        TypeId get_type_id() override;
        bool operator==(Type &other) override;

        unsigned get_byte_size() const override;
    };

    class FuncType: public Type {
    public:
        std::vector<Type *>     arg_types;
        Type                   *ret_type;

        std::string to_string() const override;

        FuncType(std::vector<Type *> arg_types, Type *ret_type);

        TypeId get_type_id() override;
        bool operator==(Type &other) override;
    };

    class Void: public Type {

        std::string to_string() const override;

        TypeId get_type_id() override;
        bool operator==(Type &other) override;
    };

    class Label: public Type {

        std::string to_string() const override;

        TypeId get_type_id() override;
        bool operator==(Type &other) override;
    };

class Value: public Base {
public:
    virtual riscv_trans::Register value_to_riscv(std::string &str) const = 0;

    virtual bool is_const() = 0;
    virtual int get_val() = 0;
};

    namespace id_type {
        enum IdType { LocalId, GlobalId, ConstId, FuncId, BlockLabel };
    }
    using IdType = id_type::IdType;

    class Id: public Value {
    public:
        IdType          id_type;
        Type           *type;
        std::string     lit;

        std::string to_string() const override;

        riscv_trans::Register value_to_riscv(std::string &str) const override;

        bool is_const() override;
        int get_val() override;

        Id(IdType id_type, Type *type, std::string lit);
        Id(IdType id_type, Type *type, std::string lit, int val);

    private:
        bool is_const_bool;
        int val;
    };

    class Const: public Value {
    public:
        std::string to_string() const override;

        riscv_trans::Register value_to_riscv(std::string &str) const override;

        bool is_const() override;
        int get_val() override;

        Const(int val);
    private:
        int val;
    };

    class Undef: public Value {

        std::string to_string() const override;

        bool is_const() override;
        int get_val() override;
    };

class Initializer: public Base {
    public:
    virtual void initializer_to_riscv(std::string &str, unsigned type_byte_size) const = 0;
};

    class ConstInitializer: public Initializer {
    public:
        int val;

        std::string to_string() const override;

        void initializer_to_riscv(std::string &str, unsigned type_byte_size) const override;

        ConstInitializer(int val);
    };

    class Aggregate: public Initializer {
    public:
        std::vector<Initializer *> initializers;

        std::string to_string() const override;

        Aggregate(std::vector<Initializer *> initializers);
    };

    class Zeroinit: public Initializer {
        std::string to_string() const override;

        void initializer_to_riscv(std::string &str, unsigned type_byte_size) const override;
    };

    class UndefInitializer: public Initializer {

        std::string to_string() const override;
    };

class Stmt: public Base {
public:
    virtual void stmt_to_riscv(
        std::string &str, 
        riscv_trans::TransMode trans_mode
    ) const = 0;

    bool is_unit;

    virtual bool is_end_stmt() = 0;
};
    class NotEndStmt: public Stmt {
        bool is_end_stmt() override;
    };

        /**
         * serve as the right value of assignment statement
         */
        class Rvalue: public Base {
        public:
            virtual riscv_trans::Register rvalue_to_riscv(
                std::string &str
            ) const = 0;
        };

            class MemoryDecl: public Rvalue {
            public:
                Type *type;

                MemoryDecl(Type *type);

                std::string to_string() const override;

                riscv_trans::Register rvalue_to_riscv(
                    std::string &str
                ) const override;
            };

            class Load: public Rvalue {
            public:
                Id *addr;

                Load(Id *addr);

                std::string to_string() const override;

                riscv_trans::Register rvalue_to_riscv(
                    std::string &str
                ) const override;
            };

            class GetPtr: public Rvalue {
            public:
                Id     *base;
                Value  *offset;

                std::string to_string() const override;

                GetPtr(Id *base, Value *offset);
            };

            class GetElemPtr: public Rvalue {
            public:
                Id     *base;
                Value  *offset;

                std::string to_string() const override;

                GetElemPtr(Id *base, Value *offset);
            };

            class Expr: public Rvalue {
            protected:
                Value  *lv;
                Value  *rv;

                Expr(Value *lv, Value *rv);
            };

                class Eq: public Expr {
                public:
                    Eq(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };
                
                class Ne: public Expr {
                public:
                    Ne(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Gt: public Expr {
                public:
                    Gt(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Lt: public Expr {
                public:
                    Lt(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Ge: public Expr {
                public:
                    Ge(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Le: public Expr {
                public:
                    Le(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Add: public Expr {
                public:
                    Add(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Sub: public Expr {
                public:
                    Sub(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Mul: public Expr {
                public:
                    Mul(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Div: public Expr {
                public:
                    Div(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Mod: public Expr {
                public:
                    Mod(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class And: public Expr {
                public:
                    And(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Or: public Expr {
                public:
                    Or(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Xor: public Expr {
                public:
                    Xor(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Shl: public Expr {
                public:
                    Shl(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Shr: public Expr {
                public:
                    Shr(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

                class Sar: public Expr {
                public:
                    Sar(Value *lv, Value *rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string &str) const override;
                };

            class FuncCall: public Rvalue, public NotEndStmt {
            public:
                Id                     *id;
                std::vector<Value *>    args;

                std::string to_string() const override;

                void stmt_to_riscv(
                    std::string &str, 
                    riscv_trans::TransMode trans_mode
                ) const override;

                riscv_trans::Register rvalue_to_riscv(
                    std::string &str
                ) const override;

                FuncCall(Id *id, std::vector<Value *> args);
            };

        class SymbolDef: public NotEndStmt {
        public:
            Id     *id;
            Rvalue *val;

            std::string to_string() const override;

            void stmt_to_riscv(
                std::string &str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            SymbolDef(Id *id, Rvalue *val);
        };

        class Store: public NotEndStmt {
        };

            class StoreValue: public Store {
            public:
                Value  *value;
                Id     *addr;

                StoreValue(Value *value, Id *addr);

                std::string to_string() const override;

                void stmt_to_riscv(
                    std::string &str, 
                    riscv_trans::TransMode trans_mode
                ) const override;
            };

            class StoreInitializer: public Store {
            public:
                Initializer    *initializer;
                Id             *addr;

                std::string to_string() const override;

                StoreInitializer(Initializer *initializer, Id *addr);
            };

    class EndStmt: public Stmt {
        bool is_end_stmt() override;
    };

        class Branch: public EndStmt {
        public:
            Value  *cond;
            Id     *target1;
            Id     *target2;

            std::string to_string() const override;

            void stmt_to_riscv(
                std::string &str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            Branch(Value *cond, Id *target1, Id *target2);
        };

        class Jump: public EndStmt {
        public:
            Id *target;

            std::string to_string() const override;

            void stmt_to_riscv(
                std::string &str, 
                riscv_trans::TransMode trans_mode
            ) const override;
            
            Jump(Id *target);
        };

        namespace return_type {
            enum ReturnType { HasRetVal, NotHasRetVal };
        }
        using ReturnType = return_type::ReturnType;

        class Return: public EndStmt {
        public:
            std::string to_string() const override;

            void stmt_to_riscv(
                std::string &str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            Return();
            Return(Value *val);
        
        private:
            ReturnType  return_type;
            Value      *val;
        };

    class GlobalStmt: public Stmt {
        bool is_end_stmt() override;
    };

        class Block: public Base {
        public:
            Id                     *id;
            std::vector<Stmt *>     stmts;

            std::vector<std::string> preds;
            std::vector<std::string> succs;

            friend void operator+=(Block &self, Stmt *stmt);

            friend void operator+=(Block &self, std::vector<Stmt *> stmts);

            std::string to_string() const override;

            void block_to_riscv(std::string &str) const;

            Block(Id *id, std::vector<Stmt *> stmts);
        };

        class FuncDef: public GlobalStmt {
        public:
            Id                     *id;
            std::vector<Id *>       formal_param_ids;
            std::vector<Block *>    blocks;

            std::string to_string() const override;

void stmt_to_riscv(
                std::string &str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            FuncDef(
                Id *id, 
                std::vector<Id *> formal_param_ids, 
                std::vector<Block *> blocks
            );
        };

        class FuncDecl: public GlobalStmt {
        public:
            Id *id;

            std::string to_string() const override;

void stmt_to_riscv(
                std::string &str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            FuncDecl(Id *id);
        };

        class GlobalMemoryDecl: public GlobalStmt {
        public:
            Type           *type;
            Initializer    *initializer;

            std::string to_string() const override;

void stmt_to_riscv(
                std::string &str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            GlobalMemoryDecl(Type *type, Initializer *initializer);
        };

        class GlobalSymbolDef: public GlobalStmt {
        public:
            Id                 *id;
            GlobalMemoryDecl   *decl;

            std::string to_string() const override;

void stmt_to_riscv(
                std::string &str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            GlobalSymbolDef(Id *id, GlobalMemoryDecl *decl);
        };

class Program: public Base {
public:
    std::vector<GlobalStmt*> global_stmts;

    std::string to_string() const override;

    void prog_to_riscv(std::string &str) const;

    Program(std::vector<GlobalStmt *> global_stmts);

};

}



#endif