#ifndef KOOPA_H_
#define KOOPA_H_

#include <iostream>
#include <vector>

namespace koopa {

class Base {
public:
    virtual std::string to_string() = 0;

    virtual ~Base() = default;
};

class Type : public Base {
public:
};

    class Int : public Type {

        std::string to_string() override;

    };

    class Array : public Type {
    public:
        Type *elem_type = nullptr;
        int length = 0;

        std::string to_string() override;

        Array(Type *elem_type, int length) : elem_type(elem_type), length(length) {}

        ~Array() override;
    };

    class Pointer : public Type {
    public:
        Type *pointed_type = nullptr;

        std::string to_string() override;

        Pointer(Type *pointed_type) : pointed_type(pointed_type) {}

        ~Pointer() override;
    };

    class FuncType : public Type {
    public:
        std::vector<Type *>     *arg_types  = nullptr;
        Type                    *ret_type   = nullptr;

        std::string to_string() override;

        FuncType(std::vector<Type *> *arg_types, Type *ret_type) :
            arg_types(arg_types), ret_type(ret_type) {}

        ~FuncType() override;
    };

    class Label : public Type {

        std::string to_string() override;

    };

    class Void : public Type {

        std::string to_string() override;

    };

class Value : public Base {
};

    class Id : public Value {
    public:
        Type        *type   = nullptr;
        std::string *lit    = nullptr;

        std::string to_string() override;

        Id(Type *type, std::string *lit) : type(type), lit(lit) {}

        ~Id() override;
    };

    class Const : public Value {
    public:
        int val = 0;

        std::string to_string() override;

        Const(int val) : val(val) {}
    };

    class Undef : public Value {

        std::string to_string() override;

    };

class Initializer : public Base {
};

    class ConstInitializer : public Initializer {
    public:
        int val = 0;

        std::string to_string() override;

        ConstInitializer(int val) : val(val) {}
    };

    class Aggregate : public Initializer {
    public:
        std::vector<Initializer *> *initializers = nullptr;

        std::string to_string() override;

        Aggregate(std::vector<Initializer *> *initializers) : initializers(initializers) {}

        ~Aggregate() override;
    };

    class Zeroinit : public Initializer {

        std::string to_string() override;

    };

    class UndefInitializer : public Initializer {

        std::string to_string() override;

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

            std::string to_string() override;

            MemoryDecl(Type *type) : type(type) {}

            ~MemoryDecl() override;
        };

        class Load : public Rvalue {
        public:
            Id *addr = nullptr;

            std::string to_string() override;

            Load(Id *addr) : addr(addr) {}

            ~Load() override;
        };

        class GetPtr : public Rvalue {
        public:
            Id      *base   = nullptr;
            Value   *offset = nullptr;

            std::string to_string() override;

            GetPtr(Id *base, Value *offset) : base(base), offset(offset) {}

            ~GetPtr() override;
        };

        class GetElemPtr : public Rvalue {
        public:
            Id      *base   = nullptr;
            Value   *offset = nullptr;

            std::string to_string() override;

            GetElemPtr(Id *base, Value *offset) : base(base), offset(offset) {}

            ~GetElemPtr() override;
        };

        namespace op {
            enum Op {
                NE, EQ, GT, LT, GE, LE, ADD, SUB, MUL,
                DIV, MOD, AND, OR, XOR, SHL, SHR, SAR,
            };  
        }

        class Expr : public Rvalue {
        public:
            op::Op op;
            Value *lv, *rv = nullptr;

            std::string to_string() override;

            Expr(op::Op op, Value *lv, Value *rv) : op(op), lv(lv), rv(rv) {}

            ~Expr() override;
        };

        class FuncCall : public Rvalue, public Stmt {
        public:
            Id                      *id     = nullptr;
            std::vector<Value *>    *args   = nullptr;

            std::string to_string() override;

            FuncCall(Id *id, std::vector<Value *> *args) : id(id), args(args) {}

            ~FuncCall() override;
        };

    class SymbolDef : public Stmt {
    public:
        Id      *id     = nullptr;
        Rvalue  *val    = nullptr;

        std::string to_string() override;

        SymbolDef(Id *id, Rvalue *val) : id(id), val(val) {}

        ~SymbolDef() override;
    };

    class Store : public Stmt {
    };

        class StoreValue : public Store {
        public:
            Value   *value  = nullptr;
            Id      *addr   = nullptr;

            std::string to_string() override;

            StoreValue(Value *value, Id *addr) : value(value), addr(addr) {}

            ~StoreValue() override;
        };

        class StoreInitializer : public Store {
        public:
            Initializer *initializer    = nullptr;
            Id          *addr           = nullptr;

            std::string to_string() override;

            StoreInitializer(Initializer *initializer, Id *addr) :
                initializer(initializer), addr(addr) {}

            ~StoreInitializer() override;
        };

class EndStmt : public Base {
};

    class Branch : public EndStmt {
    public:
        Value   *cond       = nullptr;
        Id      *target1    = nullptr;
        Id      *target2    = nullptr;

        std::string to_string() override;

        Branch(Value *cond, Id *target1, Id *target2) : cond(cond),
            target1(target1), target2(target2) {}

        ~Branch() override;
    };

    class Jump : public EndStmt {
    public:
        Id *target = nullptr;

        std::string to_string() override;

        Jump(Id *target) : target(target) {}

        ~Jump() override;
    };

    class Return : public EndStmt {
    public:
        Value *val = nullptr;

        std::string to_string() override;

        Return(Value *val) : val(val) {}

        ~Return() override;
    };

class GlobalStmt : public Base {
};

    class Block : public Base {
    public:
        Id                  *id         = nullptr;
        std::vector<Stmt *> *stmts      = nullptr;
        EndStmt             *end_stmt   = nullptr;

        std::string to_string() override;

        Block(Id *id, std::vector<Stmt *> *stmts, EndStmt *end_stmt) :
            id(id), stmts(stmts), end_stmt(end_stmt) {}

        ~Block() override;
    };

    class FuncParamDecl : public Base {
    public:
        Id      *id     = nullptr;
        Type    *type   = nullptr;

        std::string to_string() override;

        FuncParamDecl(Id *id, Type *type) :
            id(id), type(type) {}

        ~FuncParamDecl() override;
    };

    class FuncDef : public GlobalStmt {
    public:
        Id                              *id                 = nullptr;
        std::vector<FuncParamDecl *>    *func_param_decls   = nullptr;
        Type                            *ret_type           = nullptr;
        std::vector<Block *>            *blocks             = nullptr;

        std::string to_string() override;

        FuncDef(Id *id, std::vector<FuncParamDecl *> *func_param_decls,
                Type *ret_type, std::vector<Block *> *blocks) :
            id(id), func_param_decls(func_param_decls), ret_type(ret_type), blocks(blocks) {}

        ~FuncDef() override;
    };

    class FuncDecl : public GlobalStmt {
    public:
        Id                  *id             = nullptr;
        std::vector<Type *> *param_types    = nullptr;
        Type                *ret_type       = nullptr;

        std::string to_string() override;

        FuncDecl(Id *id, std::vector<Type *> *param_types, Type *ret_type) :
            id(id), param_types(param_types), ret_type(ret_type) {}

        ~FuncDecl() override;
    };

    class GlobalMemoryDecl : public Base {
    public:
        Type        *type           = nullptr;
        Initializer *initializer    = nullptr;

        std::string to_string() override;

        GlobalMemoryDecl(Type *type, Initializer *initializer) :
            type(type), initializer(initializer) {}

        ~GlobalMemoryDecl() override;
    };

    class GlobalSymbolDef : public GlobalStmt {
    public:
        Id                  *id     = nullptr;
        GlobalMemoryDecl    *decl   = nullptr;

        std::string to_string() override;

        GlobalSymbolDef(Id *id, GlobalMemoryDecl *decl) :
            id(id), decl(decl) {}

        ~GlobalSymbolDef() override;
    };

class Program : public Base {
public:
    std::vector<GlobalStmt*> *global_stmts = nullptr;

    std::string to_string() override;

    Program(std::vector<GlobalStmt *> *global_stmts) : global_stmts(global_stmts) {}

    ~Program() override;
};

}



#endif