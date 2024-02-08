#ifndef KOOPA_H_
#define KOOPA_H_

#include <iostream>
#include <vector>

namespace koopa {

class Base {
public:
    // virtual std::string to_string() = 0;
    // virtual void debug(int indent = 0) = 0;

    virtual ~Base() = default;
};

class Type : public Base {
};

    class Int : public Type {
    };

    class Array : public Type {
    public:
        Type *elem_type = nullptr;
        int length = 0;

        Array(Type *elem_type, int length) : elem_type(elem_type), length(length) {}

        ~Array() override;
    };

    class Pointer : public Type {
    public:
        Type *pointed_type = nullptr;

        Pointer(Type *pointed_type) : pointed_type(pointed_type) {}

        ~Pointer() override;
    };

    class FuncType : public Type {
    public:
        std::vector<Type *> *arg_types = nullptr;
        Type *ret_type = nullptr;

        FuncType(std::vector<Type *> *arg_types, Type *ret_type) :
            arg_types(arg_types), ret_type(ret_type) {}

        ~FuncType() override;
    };

    class Label : public Type {
    };

    class Void : public Type {
    };

class Value : public Base {
};

    class Id : public Value {
    public:
        Type *type = nullptr;
        std::string *lit = nullptr;

        Id(Type *type, std::string *lit) : type(type), lit(lit) {}

        ~Id() override;
    };

    class Const : public Value {
    public:
        int val = 0;

        Const(int val) : val(val) {}
    };

    class Undef : public Value {
    };

class Initializer : public Base {
};

    class ConstInitializer : public Initializer {
    public:
        int val = 0;

        ConstInitializer(int val) : val(val) {}
    };

    class Aggregate : public Initializer {
    public:
        std::vector<Initializer *> *initializers = nullptr;

        Aggregate(std::vector<Initializer *> *initializers) : initializers(initializers) {}

        ~Aggregate() override;
    };

    class Zeroinit : public Initializer {
    };

    class UndefInitializer : public Initializer {
    };

// Stmt and Rvalue virtually inherit Base because :
//
//#          *-> Stmt ----*-> FuncCall
//#         /            /
//#  Base -*---> Rvalue *
//
class Stmt : virtual public Base {
};

    class Rvalue : virtual public Base {
    };

        class MemoryDecl : public Rvalue {
        public:
            Type *type = nullptr;

            MemoryDecl(Type *type) : type(type) {}

            ~MemoryDecl() override;
        };

        class Load : public Rvalue {
        public:
            Id *addr = nullptr;

            Load(Id *addr) : addr(addr) {}

            ~Load() override;
        };

        class GetPtr : public Rvalue {
        public:
            Id *base = nullptr;
            Value *offset = nullptr;

            GetPtr(Id *base, Value *offset) : base(base), offset(offset) {}

            ~GetPtr() override;
        };

        class GetElemPtr : public Rvalue {
        public:
            Id *base = nullptr;
            Value *offset = nullptr;

            GetElemPtr(Id *base, Value *offset) : base(base), offset(offset) {}

            ~GetElemPtr() override;
        };

        enum BINARY_OP {
            NE, EQ, GT, LT, GE, LE, ADD, SUB, MUL,
            DIV, MOD, AND, OR, XOR, SHL, SHR, SAR,
        };

        class Expr : public Rvalue {
        public:
            BINARY_OP op;
            Value *lv, *rv = nullptr;

            Expr(BINARY_OP op, Value *lv, Value *rv) : op(op), lv(lv), rv(rv) {}

            ~Expr() override;
        };

        class FuncCall : public Rvalue, public Stmt {
        public:
            Id *id = nullptr;
            std::vector<Value *> *args = nullptr;

            FuncCall(Id *id, std::vector<Value *> *args) : id(id), args(args) {}

            ~FuncCall() override;
        };

    class SymbolDef : public Stmt {
    public:
        Id *id = nullptr;
        Rvalue *val = nullptr;

        SymbolDef(Id *id, Rvalue *val) : id(id), val(val) {}

        ~SymbolDef() override;
    };

    class Store : public Stmt {
    };

        class StoreValue : public Store {
        public:
            Value *value = nullptr;
            Id *addr = nullptr;

            StoreValue(Value *value, Id *addr) : value(value), addr(addr) {}

            ~StoreValue() override;
        };

        class StoreInitializer : public Store {
        public:
            Initializer *initializer = nullptr;
            Id *addr = nullptr;

            StoreInitializer(Initializer *initializer, Id *addr) :
                initializer(initializer), addr(addr) {}

            ~StoreInitializer() override;
        };

class EndStmt : public Base {
};

    class Branch : public EndStmt {
    public:
        Value *cond = nullptr;
        Id *target[2] = { nullptr, nullptr };

        Branch(Value *cond, Id *target[2]) : cond(cond) {
            this->target[0] = target[0];
            this->target[1] = target[1];
        }

        ~Branch() override;
    };

    class Jump : public EndStmt {
    public:
        Id *target = nullptr;

        Jump(Id *target) : target(target) {}

        ~Jump() override;
    };

    class Return : public EndStmt {
    public:
        Value *val = nullptr;

        Return(Value *val) : val(val) {}

        ~Return() override;
    };

class GlobalStmt : public Base {
};

    class Block : public Base {
    public:
        std::vector<Stmt *> *stmts = nullptr;
        EndStmt *end_stmt = nullptr;

        Block(std::vector<Stmt *> *stmts, EndStmt *end_stmt) :
            stmts(stmts), end_stmt(end_stmt) {}

        ~Block() override;
    };

    class FuncParamDecl : public Base {
    public:
        Id *id = nullptr;
        Type *type = nullptr;

        FuncParamDecl(Id *id, Type *type) :
            id(id), type(type) {}

        ~FuncParamDecl() override;
    };

    class FuncDef : public GlobalStmt {
    public:
        Id *id = nullptr;
        std::vector<FuncParamDecl *> *func_param_decls = nullptr;
        Type *ret_type = nullptr;
        std::vector<Block *> *blocks = nullptr;

        FuncDef(Id *id, std::vector<FuncParamDecl *> *func_param_decls,
                Type *ret_type, std::vector<Block *> *blocks) :
            id(id), func_param_decls(func_param_decls), ret_type(ret_type), blocks(blocks) {}

        ~FuncDef() override;
    };

    class FuncDecl : public GlobalStmt {
    public:
        Id *id = nullptr;
        std::vector<Type *> *param_types = nullptr;
        Type *ret_type = nullptr;

        FuncDecl(Id *id, std::vector<Type *> *param_types, Type *ret_type) :
            id(id), param_types(param_types), ret_type(ret_type) {}

        ~FuncDecl() override;
    };

    class GlobalMemoryDecl : public Base {
    public:
        Type *type = nullptr;
        Initializer *initializer = nullptr;

        GlobalMemoryDecl(Type *type, Initializer *initializer) :
            type(type), initializer(initializer) {}

        ~GlobalMemoryDecl() override;
    };

    class GlobalSymbolDef : public GlobalStmt {
    public:
        Id *id = nullptr;
        GlobalMemoryDecl *decl = nullptr;

        GlobalSymbolDef(Id *id, GlobalMemoryDecl *decl) :
            id(id), decl(decl) {}

        ~GlobalSymbolDef() override;
    };

class Program : public Base {
public:
    std::vector<GlobalStmt*> *global_stmts = nullptr;

    Program(std::vector<GlobalStmt *> *global_stmts) : global_stmts(global_stmts) {}

    ~Program() override;
};

}



#endif