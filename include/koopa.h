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


class Type: public Base {
public:
    enum TypeId { Int, Array, Pointer, FuncType, Void };

    virtual TypeId get_type_id() = 0;
    bool operator==(Type& other);
    virtual bool operator==(Type&& other) = 0;
    bool operator!=(Type& other);
    bool operator!=(Type&& other);

    /**
     * @return type dimensions, use -1 to represent pointer
     * @example int => {}
     * @example int[][2].get_dim() => { -1, 2 } 
     */
    virtual std::vector<int> get_dim() const;

    virtual unsigned get_byte_size() const;

    /**
     * @example int[2][3].unwrap() => int[3]
     * crash when unwrapping not unwrappable type
     */
    virtual Type* unwrap() const;
};

    class Int: public Type {

        std::string to_string() const override;

        TypeId get_type_id() override;
        bool operator==(Type&& other) override;

        std::vector<int> get_dim() const override;

        unsigned get_byte_size() const override;
    };

    class Array: public Type {
    public:
        std::string to_string() const override;

        Array(Type* elem_type, int length);

        TypeId get_type_id() override;
        bool operator==(Type&& other) override;

        std::vector<int> get_dim() const override;

        unsigned get_byte_size() const override;

        Type* unwrap() const override;

    private:
        Type* elem_type;
        int length;
    };

    class Pointer: public Type {
    public:
        std::string to_string() const override;

        Pointer(Type* pointed_type);

        TypeId get_type_id() override;
        bool operator==(Type&& other) override;

        std::vector<int> get_dim() const override;

        unsigned get_byte_size() const override;

        Type* unwrap() const override;

    private:
        Type* pointed_type;
    };

    class FuncType: public Type {
    public:
        std::string to_string() const override;

        FuncType(std::vector<Type*> arg_types, Type* ret_type);

        TypeId get_type_id() override;
        bool operator==(Type&& other) override;

        std::vector<Type*> get_arg_types() const;
        Type* get_ret_type() const;

    private:
        std::vector<Type*> arg_types;
        Type* ret_type;
    };

    class Void: public Type {

        std::string to_string() const override;

        TypeId get_type_id() override;
        bool operator==(Type&& other) override;
    };

class Value: public Base {
public:
    virtual riscv_trans::Register value_to_riscv(std::string& str) const = 0;

    virtual bool is_const() = 0;
    virtual int get_val() = 0;
};


    class Id: public Value {
    public:
        std::string to_string() const override;

        riscv_trans::Register value_to_riscv(std::string& str) const override;

        bool is_const() override;
        int get_val() override;

        Id(Type* type, std::string lit);
        Id(Type* type, std::string lit, int val);

        Type* get_type() const;
        std::string get_lit() const;

    private:
        Type* type;
        std::string lit;
        bool is_const_bool;
        int val;
    };

    class Const: public Value {
    public:
        std::string to_string() const override;

        riscv_trans::Register value_to_riscv(std::string& str) const override;

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
    virtual std::vector<int> to_flat_vec(unsigned type_byte_size) const = 0;
};

    class ConstInitializer: public Initializer {
    public:
        std::string to_string() const override;

        std::vector<int> to_flat_vec(unsigned type_byte_size) const override;

        ConstInitializer(int val);

        int get_val() const;

    private:
        int val;
    };

    class Aggregate: public Initializer {
    public:
        std::vector<Initializer*> initializers;

        std::string to_string() const override;

        std::vector<int> to_flat_vec(unsigned type_byte_size) const override;

        Aggregate(std::vector<Initializer*> initializers);
    };

    class Zeroinit: public Initializer {
        std::string to_string() const override;

        std::vector<int> to_flat_vec(unsigned type_byte_size) const override;
    };

    class UndefInitializer: public Initializer {

        std::string to_string() const override;
    };

class Label {
public:
    Label();
    Label(std::string name);
    std::string get_name() const;

private:
    std::string name;
};

class Stmt: public Base {
public:
    virtual void stmt_to_riscv(
        std::string& str, 
        riscv_trans::TransMode trans_mode
    ) const = 0;

    virtual bool is_end_stmt() = 0;

    virtual bool is_func_call() const;
    /* crash if is not a function calling */
    virtual unsigned get_func_call_param_n() const;
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
                std::string& str
            ) const = 0;

            virtual bool is_func_call() const;
            /* crash if is not a function calling */
            virtual unsigned get_func_call_param_n() const;
        };

            /**
             * We need to allocate a physical stack frame for the alloc instruction
             * and identify it with a pseudo ID. Variables are pointers that hold 
             * the address of the stack frame and have their own stack frame space. 
             * When accessing or setting variables, an additional dereference is 
             * performed. Constructor functions take literals of variables, and 
             * the function automatically adds a unique marker to them. Although 
             * this unnecessarily increases memory I/O, it ensures consistent behavior 
             * for load and store instructions. This behavior can degrade to regular 
             * access and setting operations in constant propagation optimization.
             */
            class MemoryDecl: public Rvalue {
            public:
                /**
                 * @param pseudo_lit  raw literal of variable, for example `x`
                 */
                MemoryDecl(
                    Type* type, std::string pseudo_lit, 
                    NestingInfo* nesting_info = new NestingInfo
                );

                std::string to_string() const override;

                riscv_trans::Register rvalue_to_riscv(
                    std::string& str
                ) const override;

            private:
                Id* pseudo_id;
                Type* type;
            };

            class Load: public Rvalue {
            public:
                Load(Id* addr);

                std::string to_string() const override;

                riscv_trans::Register rvalue_to_riscv(
                    std::string& str
                ) const override;

            private:
                Id* addr;
            };

            class GetPtr: public Rvalue {
            public:
                std::string to_string() const override;

                GetPtr(Id* base, Value* offset);

                riscv_trans::Register rvalue_to_riscv(
                    std::string& str
                ) const override;

            private:
                Id* base;
                Value* offset;
            };

            class GetElemPtr: public Rvalue {
            public:
                GetElemPtr(Id* base, Value* offset);

                std::string to_string() const override;

                riscv_trans::Register rvalue_to_riscv(
                    std::string& str
                ) const override;
            
            private:
                Id* base;
                Value* offset;
            };

            class Expr: public Rvalue {
            protected:
                Value* lv;
                Value* rv;

                Expr(Value* lv, Value* rv);
            };

                class Eq: public Expr {
                public:
                    Eq(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };
                
                class Ne: public Expr {
                public:
                    Ne(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Gt: public Expr {
                public:
                    Gt(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Lt: public Expr {
                public:
                    Lt(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Ge: public Expr {
                public:
                    Ge(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Le: public Expr {
                public:
                    Le(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Add: public Expr {
                public:
                    Add(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Sub: public Expr {
                public:
                    Sub(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Mul: public Expr {
                public:
                    Mul(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Div: public Expr {
                public:
                    Div(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Mod: public Expr {
                public:
                    Mod(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class And: public Expr {
                public:
                    And(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Or: public Expr {
                public:
                    Or(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Xor: public Expr {
                public:
                    Xor(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Shl: public Expr {
                public:
                    Shl(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Shr: public Expr {
                public:
                    Shr(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

                class Sar: public Expr {
                public:
                    Sar(Value* lv, Value* rv);

                    std::string to_string() const override;

                    riscv_trans::Register rvalue_to_riscv(std::string& str) const override;
                };

            class FuncCall: public Rvalue, public NotEndStmt {
            public:
                std::string to_string() const override;

                void stmt_to_riscv(
                    std::string& str, 
                    riscv_trans::TransMode trans_mode
                ) const override;

                riscv_trans::Register rvalue_to_riscv(
                    std::string& str
                ) const override;

                bool is_func_call() const override;
                unsigned get_func_call_param_n() const override;

                FuncCall(Id* id, std::vector<Value*> args);

                Id* get_id() const;
                std::vector<Value*> get_args() const;

            private:
                Id* id;
                std::vector<Value*> args;
            };

        class SymbolDef: public NotEndStmt {
        public:
            std::string to_string() const override;

            void stmt_to_riscv(
                std::string& str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            bool is_func_call() const override;
            unsigned get_func_call_param_n() const override;

            SymbolDef(Id* id, Rvalue* val);

            Rvalue* get_val() const;

        private:
            Id* id;
            Rvalue* val;
        };

        class Store: public NotEndStmt {
        };

            class StoreValue: public Store {
            public:
                StoreValue(Value* value, Id* addr);

                std::string to_string() const override;

                void stmt_to_riscv(
                    std::string& str, 
                    riscv_trans::TransMode trans_mode
                ) const override;

            private:
                Value* value;
                Id* addr;
            };

            class StoreInitializer: public Store {
            public:
                std::string to_string() const override;

                void stmt_to_riscv(
                    std::string& str, 
                    riscv_trans::TransMode trans_mode
                ) const override;

                StoreInitializer(Initializer* initializer, Id* addr);

            private:
                Initializer* initializer;
                Id* addr;
            };

    class EndStmt: public Stmt {
        bool is_end_stmt() override;
    };

        class Branch: public EndStmt {
        public:
            std::string to_string() const override;

            void stmt_to_riscv(
                std::string& str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            Branch(Value* cond, Label target1, Label target2);

        private:
            Value* cond;
            Label target1;
            Label target2;
        };

        class Jump: public EndStmt {
        public:
            std::string to_string() const override;

            void stmt_to_riscv(
                std::string& str, 
                riscv_trans::TransMode trans_mode
            ) const override;
            
            Jump(Label target);

        private:
            Label target;
        };


        class Return: public EndStmt {
        public:
            enum ReturnType { HasRetVal, NotHasRetVal };

            std::string to_string() const override;

            void stmt_to_riscv(
                std::string& str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            Return();
            Return(Value* val);
        
        private:
            ReturnType return_type;
            Value* val;
        };

    class GlobalStmt: public Stmt {
        bool is_end_stmt() override;
    };

        class Block: public Base {
        public:
            friend void operator+=(Block& self, Stmt* stmt);

            friend void operator+=(Block& self, std::vector<Stmt*> stmts);

            std::string to_string() const override;

            void block_to_riscv(std::string& str) const;

            Block(Label label, std::vector<Stmt*> stmts);

            Label get_label() const;
            std::vector<Stmt*>& get_stmts();

        private:
            Label label;
            std::vector<Stmt*> stmts;
        };

        class FuncDef: public GlobalStmt {
        public:
            std::string to_string() const override;

            /**
             * koopa only allows functions declared or defined once,
             * so the declaration must expanse to corresponding definition
             */
            std::string func_decl_to_string_agent() const;

            void stmt_to_riscv(
                std::string& str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            FuncDef(
                Id* id, 
                std::vector<Id*> formal_param_ids, 
                std::vector<Block*> blocks
            );

            Id* get_id() const;
            std::vector<Id*> get_formal_param_ids() const;
            std::vector<Block*> get_blocks() const;

        private:
            Id* id;
            std::vector<Id*> formal_param_ids;
            std::vector<Block*> blocks;
        };

        class FuncDecl: public GlobalStmt {
        public:
            std::string to_string() const override;

            void stmt_to_riscv(
                std::string& str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            FuncDecl(Id* id);

            static std::unordered_set<koopa::Id*> declared_funcs;
            static std::unordered_map<koopa::Id*, koopa::FuncDef*> func_implementations;

        private:
            Id* id;
        };

        class GlobalMemoryDecl: public GlobalStmt {
        public:
            std::string to_string() const override;

            void stmt_to_riscv(
                std::string& str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            GlobalMemoryDecl(Type* type, Initializer* initializer);

        private:
            Type* type;
            Initializer* initializer;
        };

        class GlobalSymbolDef: public GlobalStmt {
        public:
            std::string to_string() const override;

            void stmt_to_riscv(
                std::string& str, 
                riscv_trans::TransMode trans_mode
            ) const override;

            GlobalSymbolDef(Id* id, GlobalMemoryDecl* decl);

        private:
            Id* id;
            GlobalMemoryDecl* decl;
        };

class Program: public Base {
public:
    std::string to_string() const override;

    void prog_to_riscv(std::string& str) const;

    Program(std::vector<GlobalStmt*> global_stmts);

private:
    std::vector<GlobalStmt*> global_stmts;
};

}



#endif