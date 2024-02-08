#ifndef KOOPA_H_
#define KOOPA_H_

#include <iostream>
#include <vector>

namespace koopa {

class Base {
public:
    virtual ~Base() = default;
};

class Type : public Base {
};

    class Int : public Type {
    };

    class Array : public Type {
    public:
        Type *elem_type = nullptr;
        int length;

        ~Array() override;
    };

    class Pointer : public Type {
    public:
        Type *pointed_type = nullptr;

        ~Pointer() override;
    };

    class Func : public Type {
    public:
        std::vector<Type *> *arg_types = nullptr;
        Type *ret_type = nullptr;

        ~Func() override;
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

        ~Id() override;
    };

    class Const : public Value {
    public:
        int val;
    };

    class Undef : public Value {
    };

class Initializer : public Base {
};

    class ConstInitializer : public Initializer {
    public:
        int val;
    };

    class Aggregate : public Initializer {
    public:
        std::vector<Initializer *> *initializers = nullptr;

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

            ~MemoryDecl() override;
        };

        class Load : public Rvalue {
        public:
            Id *addr = nullptr;

            ~Load() override;
        };

        class GetPtr : public Rvalue {
        public:
            Id *base = nullptr;
            Value *offset = nullptr;

            ~GetPtr() override;
        };

        class GetElemPtr : public Rvalue {
        public:
            Id *base = nullptr;
            Value *offset = nullptr;

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

            ~Expr() override;
        };

        class FuncCall : public Rvalue, public Stmt {
        public:
            Id *func = nullptr;
            std::vector<Value *> *args = nullptr;

            ~FuncCall() override;
        };

    class SymbolDef : public Stmt {
    public:
        Id *id = nullptr;
        Rvalue *val = nullptr;

        ~SymbolDef() override;
    };

    class Store : public Stmt {
    };

        class StoreValue : public Store {
        public:
            Value *value = nullptr;
            Id *addr = nullptr;

            ~StoreValue() override;   
        };

        class StoreInitializer : public Store {
        public:
            Initializer *initializer = nullptr;
            Id *addr = nullptr;

            ~StoreInitializer() override;
        };

class EndStmt : Base {
};

    class Branch : public EndStmt {
    public:
        Value *cond = nullptr;
        Id *target[2] = { nullptr, nullptr };

        ~Branch() override;
    };

    class Jump : public EndStmt {
    public:
        Id *target = nullptr;

        ~Jump() override;
    };

    class Return : public EndStmt {
    public:
        Value *val = nullptr;

        ~Return() override;
    };

class GlobalStmt : public Base {
};

    class Block : public Base {
    public:
        std::vector<Stmt *> *stmts = nullptr;
        EndStmt *end_stmt = nullptr;

        ~Block() override;
    };

    class FuncParamDecl : public Base {
    public:
        Id *id = nullptr;
        Type *type = nullptr;

        ~FuncParamDecl() override;
    };

    class FuncDef : public GlobalStmt {
    public:
        Id *id = nullptr;
        std::vector<FuncParamDecl *> *func_param_decls = nullptr;
        Type *ret_type = nullptr;
        std::vector<Block *> *blocks = nullptr;

        ~FuncDef() override;
    };

    class FuncDecl : public GlobalStmt {
        Id *id = nullptr;
        std::vector<Type *> *param_types = nullptr;
        Type *ret_type = nullptr;

        ~FuncDecl() override; 
    };

    class GlobalMemoryDecl : public Base {
    public:
        Type *type = nullptr;
        Initializer *Initializer = nullptr;  

        ~GlobalMemoryDecl() override;
    };

    class GlobalSymbolDef : public GlobalStmt {
    public:
        Id *id = nullptr;
        GlobalMemoryDecl *decl = nullptr;

        ~GlobalSymbolDef() override;
    };

class Program : public Base {
    std::vector<GlobalStmt*> *global_stmts = nullptr;

    ~Program() override;
};

}



#endif