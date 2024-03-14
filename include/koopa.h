#ifndef KOOPA_H_
#define KOOPA_H_

#include "nesting_info.h"
#include "trans.h"

#include <iostream>
#include <assert.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace koopa {

class Base {
public:
    virtual std::string to_string() const = 0;

    virtual void to_riscv(std::string &str, riscv_trans::Info &info) const = 0;

    virtual ~Base() = default;
};

namespace type {
    enum TypeId { Int, Array, Pointer, FuncType, Void, Label };
}

class Type : public Base {
public:
    void to_riscv(std::string &str, riscv_trans::Info &info) const override {}

    virtual type::TypeId get_type_id() = 0;
    virtual bool operator==(Type &other) = 0;
    bool operator!=(Type &other);
};

    class Int : public Type {

        std::string to_string() const override;

        type::TypeId get_type_id() override;
        bool operator==(Type &other) override;

    };

    class Array : public Type {
    public:
        Type *elem_type = nullptr;
        int length = 0;

        std::string to_string() const override;

        Array(Type *elem_type, int length);

        type::TypeId get_type_id() override;
        bool operator==(Type &other) override;

        ~Array() override;
    };

    class Pointer : public Type {
    public:
        Type *pointed_type = nullptr;

        std::string to_string() const override;

        Pointer(Type *pointed_type);

        type::TypeId get_type_id() override;
        bool operator==(Type &other) override;

        ~Pointer() override;
    };

    class FuncType : public Type {
    public:
        std::vector<Type *>     arg_types   = {};
        Type                    *ret_type   = nullptr;

        std::string to_string() const override;

        FuncType(std::vector<Type *> arg_types, Type *ret_type);

        type::TypeId get_type_id() override;
        bool operator==(Type &other) override;

        ~FuncType() override;
    };

    class Void : public Type {

        std::string to_string() const override;

        type::TypeId get_type_id() override;
        bool operator==(Type &other) override;

    };

    class Label : public Type {

        std::string to_string() const override;

        type::TypeId get_type_id() override;
        bool operator==(Type &other) override;

    };

class Value : public Base {
public:
    bool is_const = false;
    int val = 0;
};

    class Id : public Value {
    public:
        Type        *type   = nullptr;
        std::string *lit    = nullptr;

        int sf_offset = 0;

        std::string to_string() const override;

        void to_riscv(std::string &str, riscv_trans::Info &info) const override;

        Id(Type *type, std::string *lit, bool is_const = false, int val = 0);

        ~Id() override;
    };

    class Const : public Value {
    public:
        std::string to_string() const override;

        void to_riscv(std::string &str, riscv_trans::Info &info) const override;

        Const(int val);
    };

    class Undef : public Value {

        std::string to_string() const override;

    };

class Initializer : public Base {
};

    class ConstInitializer : public Initializer {
    public:
        int val = 0;

        std::string to_string() const override;

        void to_riscv(std::string &str, riscv_trans::Info &info) const override;

        ConstInitializer(int val);
    };

    class Aggregate : public Initializer {
    public:
        std::vector<Initializer *> initializers = {};

        std::string to_string() const override;

        Aggregate(std::vector<Initializer *> initializers);

        ~Aggregate() override;
    };

    class Zeroinit : public Initializer {

        std::string to_string() const override;

    };

    class UndefInitializer : public Initializer {

        std::string to_string() const override;

    };

class Stmt : public Base {
public:
    bool is_unit = true;

    virtual bool is_end_stmt() = 0;
};
    class NotEndStmt : public Stmt {
        bool is_end_stmt() override;
    };

        class Rvalue : public Base {
        };

            class MemoryDecl : public Rvalue {
            public:
                Type *type = nullptr;

                MemoryDecl(Type *type);

                std::string to_string() const override;

                void to_riscv(std::string &str, riscv_trans::Info &info) const override;

                ~MemoryDecl() override;
            };

            class Load : public Rvalue {
            public:
                Id *addr = nullptr;

                Load(Id *addr);

                std::string to_string() const override;

                void to_riscv(std::string &str, riscv_trans::Info &info) const override;

                ~Load() override;
            };

            class GetPtr : public Rvalue {
            public:
                Id      *base   = nullptr;
                Value   *offset = nullptr;

                std::string to_string() const override;

                GetPtr(Id *base, Value *offset);

                ~GetPtr() override;
            };

            class GetElemPtr : public Rvalue {
            public:
                Id      *base   = nullptr;
                Value   *offset = nullptr;

                std::string to_string() const override;

                GetElemPtr(Id *base, Value *offset);

                ~GetElemPtr() override;
            };

            namespace op {
                enum Op {
                    NE, EQ, GT, LT, GE, LE, ADD, SUB, MUL,
                    DIV, MOD, AND, OR, XOR, SHL, SHR, SAR,
                };  

                extern std::function<int(int, int)> op_func[17];
            }

            class Expr : public Rvalue {
            public:
                op::Op op;
                Value *lv, *rv = nullptr;

                bool is_const = false;
                int val = 0;

                std::string to_string() const override;

                void to_riscv(std::string &str, riscv_trans::Info &info) const override;

                Expr(op::Op op, Value *lv, Value *rv, bool is_const = false);

                ~Expr() override;
            };

            class FuncCall : public Rvalue, public NotEndStmt {
            public:
                Id                     *id     = nullptr;
                std::vector<Value *>    args    = {};

                std::string to_string() const override;

                void to_riscv(std::string &str, riscv_trans::Info &info) const override;

                FuncCall(Id *id, std::vector<Value *> args);

                ~FuncCall() override;
            };

        class SymbolDef : public NotEndStmt {
        public:
            Id      *id     = nullptr;
            Rvalue  *val    = nullptr;

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;

            SymbolDef(Id *id, Rvalue *val);

            ~SymbolDef() override;
        };

        class Store : public NotEndStmt {
        };

            class StoreValue : public Store {
            public:
                Value   *value  = nullptr;
                Id      *addr   = nullptr;

                StoreValue(Value *value, Id *addr);

                std::string to_string() const override;

                void to_riscv(std::string &str, riscv_trans::Info &info) const override;

                ~StoreValue() override;
            };

            class StoreInitializer : public Store {
            public:
                Initializer *initializer    = nullptr;
                Id          *addr           = nullptr;

                std::string to_string() const override;

                StoreInitializer(Initializer *initializer, Id *addr);

                ~StoreInitializer() override;
            };

    class EndStmt : public Stmt {
        bool is_end_stmt() override;
    };

        class Branch : public EndStmt {
        public:
            Value   *cond       = nullptr;
            Id      *target1    = nullptr;
            Id      *target2    = nullptr;

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;

            Branch(Value *cond, Id *target1, Id *target2);

            ~Branch() override;
        };

        class Jump : public EndStmt {
        public:
            Id *target = nullptr;

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;
            
            Jump(Id *target);

            ~Jump() override;
        };

        namespace return_type {
            enum ReturnType { HasRetVal, NotHasRetVal };
        }

        class Return : public EndStmt {
        public:
            return_type::ReturnType return_type;
            Value *val = nullptr;

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;

            Return();
            Return(Value *val);

            ~Return() override;
        };

    class GlobalStmt : public Stmt {
        bool is_end_stmt() override;
    };

        class Block : public Base {
        public:
            Id                  *id         = nullptr;
            std::vector<Stmt *> stmts       = {};

            std::vector<std::string> preds;
            std::vector<std::string> succs;

            int get_stack_frame_size();

            void set_id_offset(int &offset);

            friend void operator+=(Block &self, Stmt *stmt);

            friend void operator+=(Block &self, std::vector<Stmt *> stmts);

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;

            Block(Id *id, std::vector<Stmt *> stmts);

            ~Block() override;
        };

        class FuncDef : public GlobalStmt {
        public:
            Id                     *id                  = nullptr;
            std::vector<Id *>       formal_param_ids    = {};
            std::vector<Block *>    blocks              = {};

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;

            FuncDef(Id *id, std::vector<Id *> formal_param_ids, std::vector<Block *> blocks);

            ~FuncDef() override;
        };

        class FuncDecl : public GlobalStmt {
        public:
            Id                  *id             = nullptr;

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;

            FuncDecl(Id *id);

            ~FuncDecl() override;
        };

        class GlobalMemoryDecl : public Base {
        public:
            Type        *type           = nullptr;
            Initializer *initializer    = nullptr;

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;

            GlobalMemoryDecl(Type *type, Initializer *initializer);

            ~GlobalMemoryDecl() override;
        };

        class GlobalSymbolDef : public GlobalStmt {
        public:
            Id                  *id     = nullptr;
            GlobalMemoryDecl    *decl   = nullptr;

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;

            GlobalSymbolDef(Id *id, GlobalMemoryDecl *decl);

            ~GlobalSymbolDef() override;
        };

class Program : public Base {
public:
    std::vector<GlobalStmt*> global_stmts;

    std::string to_string() const override;

    void to_riscv(std::string &str, riscv_trans::Info &info) const override;

    Program(std::vector<GlobalStmt *> global_stmts);

    ~Program() override;
};

}



#endif