#ifndef KOOPA_H_
#define KOOPA_H_

#include <iostream>
#include <vector>

namespace koopa {

class Base {
public:
    Base *pa = nullptr;

    virtual std::string to_string() const = 0;

    virtual std::string to_riscv() const = 0;

    virtual ~Base() = default;
};

class Type : public Base {
public:
    std::string to_riscv() const override { return ""; }
};

    class Int : public Type {

        std::string to_string() const override;

    };

    class Array : public Type {
    public:
        Type *elem_type = nullptr;
        int length = 0;

        std::string to_string() const override;

        Array(Type *elem_type, int length) : elem_type(elem_type), length(length) {
            elem_type->pa = this;
        }

        ~Array() override;
    };

    class Pointer : public Type {
    public:
        Type *pointed_type = nullptr;

        std::string to_string() const override;

        Pointer(Type *pointed_type) : pointed_type(pointed_type) {
            pointed_type->pa = this;
        }

        ~Pointer() override;
    };

    class FuncType : public Type {
    public:
        std::vector<Type *>     arg_types   = {};
        Type                    *ret_type   = nullptr;

        std::string to_string() const override;

        FuncType(std::vector<Type *> arg_types, Type *ret_type) :
            arg_types(arg_types), ret_type(ret_type) {

            for (auto arg_type : arg_types) arg_type->pa = this;
            ret_type->pa = this;

        }

        ~FuncType() override;
    };

    class Label : public Type {

        std::string to_string() const override;

    };

    class Void : public Type {

        std::string to_string() const override;

    };

class Value : public Base {
};

    class Id : public Value {
    public:
        Type        *type   = nullptr;
        std::string *lit    = nullptr;

        std::string to_string() const override;

        std::string to_riscv() const override;

        Id(Type *type, std::string *lit) : type(type), lit(lit) {
            type->pa = this;
        }

        ~Id() override;
    };

    class Const : public Value {
    public:
        int val = 0;

        std::string to_string() const override;

        std::string to_riscv() const override;

        Const(int val) : val(val) {}
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

        ConstInitializer(int val) : val(val) {}
    };

    class Aggregate : public Initializer {
    public:
        std::vector<Initializer *> initializers = {};

        std::string to_string() const override;

        Aggregate(std::vector<Initializer *> initializers) : initializers(initializers) {

            for (auto initializer : initializers) initializer->pa = this;

        }

        ~Aggregate() override;
    };

    class Zeroinit : public Initializer {

        std::string to_string() const override;

    };

    class UndefInitializer : public Initializer {

        std::string to_string() const override;

    };

// Stmt and Rvalue virtually inherit Base because :
//
//#          *-> Stmt ----*-> FuncCall
//#         /            /
//#  Base -*---> Rvalue *
//

class Stmt : public Base {
};
    class NotEndStmt : public Stmt {
    };

        class Rvalue : public Base {
        };

            class MemoryDecl : public Rvalue {
            public:
                Type *type = nullptr;

                std::string to_string() const override;

                MemoryDecl(Type *type) : type(type) {

                    type->pa = this;

                }

                ~MemoryDecl() override;
            };

            class Load : public Rvalue {
            public:
                Id *addr = nullptr;

                std::string to_string() const override;

                Load(Id *addr) : addr(addr) {

                    addr->pa = this;

                }

                ~Load() override;
            };

            class GetPtr : public Rvalue {
            public:
                Id      *base   = nullptr;
                Value   *offset = nullptr;

                std::string to_string() const override;

                GetPtr(Id *base, Value *offset) : base(base), offset(offset) {

                    base->pa = this;
                    offset->pa = this;

                }

                ~GetPtr() override;
            };

            class GetElemPtr : public Rvalue {
            public:
                Id      *base   = nullptr;
                Value   *offset = nullptr;

                std::string to_string() const override;

                GetElemPtr(Id *base, Value *offset) : base(base), offset(offset) {

                    base->pa = this;
                    offset->pa = this;

                }

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

                std::string to_string() const override;

                std::string to_riscv() const override;

                Expr(op::Op op, Value *lv, Value *rv) : op(op), lv(lv), rv(rv) {

                    lv->pa = this;
                    rv->pa = this;

                }

                ~Expr() override;
            };

            class FuncCall : public Rvalue, public NotEndStmt {
            public:
                Id                      *id     = nullptr;
                std::vector<Value *>    args    = {};

                std::string to_string() const override;

                FuncCall(Id *id, std::vector<Value *> args) : id(id), args(args) {

                    auto base_this = static_cast<Base *>(static_cast<NotEndStmt *>(this));

                    id->pa = base_this;
                    for (auto arg : args) arg->pa = base_this;

                }

                ~FuncCall() override;
            };

        class SymbolDef : public NotEndStmt {
        public:
            Id      *id     = nullptr;
            Rvalue  *val    = nullptr;

            std::string to_string() const override;

            std::string to_riscv() const override;

            SymbolDef(Id *id, Rvalue *val) : id(id), val(val) {

                id->pa = this;
                val->pa = this;

            }

            ~SymbolDef() override;
        };

        class Store : public NotEndStmt {
        };

            class StoreValue : public Store {
            public:
                Value   *value  = nullptr;
                Id      *addr   = nullptr;

                std::string to_string() const override;

                StoreValue(Value *value, Id *addr) : value(value), addr(addr) {

                    value->pa = this;
                    addr->pa = this;

                }

                ~StoreValue() override;
            };

            class StoreInitializer : public Store {
            public:
                Initializer *initializer    = nullptr;
                Id          *addr           = nullptr;

                std::string to_string() const override;

                StoreInitializer(Initializer *initializer, Id *addr) :
                    initializer(initializer), addr(addr) {

                    initializer->pa = this;
                    addr->pa = this;

                }

                ~StoreInitializer() override;
            };

    class EndStmt : public Stmt {
    };

        class Branch : public EndStmt {
        public:
            Value   *cond       = nullptr;
            Id      *target1    = nullptr;
            Id      *target2    = nullptr;

            std::string to_string() const override;

            Branch(Value *cond, Id *target1, Id *target2) : 
                cond(cond), target1(target1), target2(target2) {

                cond->pa = this;
                target1->pa = this;
                target2->pa = this;

            }

            ~Branch() override;
        };

        class Jump : public EndStmt {
        public:
            Id *target = nullptr;

            std::string to_string() const override;

            Jump(Id *target) : target(target) {

                target->pa = this;

            }

            ~Jump() override;
        };

        class Return : public EndStmt {
        public:
            Value *val = nullptr;

            std::string to_string() const override;

            std::string to_riscv() const override;

            Return(Value *val) : val(val) {

                val->pa = this;

            }

            ~Return() override;
        };

    class GlobalStmt : public Stmt {
    };

        class Block : public Base {
        public:
            Id                  *id         = nullptr;
            std::vector<Stmt *> stmts       = {};

            std::vector<std::string> preds;
            std::vector<std::string> succs;

            std::string to_string() const override;

            std::string to_riscv() const override;

            Block(Id *id, std::vector<Stmt *> stmts) :
                id(id), stmts(stmts) {

                id->pa = this;
                for (auto stmt : stmts) stmt->pa = this;

            }

            ~Block() override;
        };

        class FuncParamDecl : public Base {
        public:
            Id      *id     = nullptr;
            Type    *type   = nullptr;

            std::string to_string() const override;

            FuncParamDecl(Id *id, Type *type) :
                id(id), type(type) {

                id->pa = this;
                type->pa = this;

            }

            ~FuncParamDecl() override;
        };

        class FuncDef : public GlobalStmt {
        public:
            Id                              *id                 = nullptr;
            std::vector<FuncParamDecl *>    func_param_decls    = {};
            Type                            *ret_type           = nullptr;
            std::vector<Block *>            blocks              = {};

            std::string to_string() const override;

            std::string to_riscv() const override;

            FuncDef(Id *id, std::vector<FuncParamDecl *> func_param_decls,
                    Type *ret_type, std::vector<Block *> blocks) :
                id(id), func_param_decls(func_param_decls), ret_type(ret_type), blocks(blocks) {

                id->pa = this;
                for (auto func_param_decl : func_param_decls) func_param_decl->pa = this;
                ret_type->pa = this;
                for (auto block : blocks) block->pa = this;

            }

            ~FuncDef() override;
        };

        class FuncDecl : public GlobalStmt {
        public:
            Id                  *id             = nullptr;
            std::vector<Type *> param_types     = {};
            Type                *ret_type       = nullptr;

            std::string to_string() const override;

            FuncDecl(Id *id, std::vector<Type *> param_types, Type *ret_type) :
                id(id), param_types(param_types), ret_type(ret_type) {

                id->pa = this;
                for (auto param_type : param_types) param_type->pa = this;
                ret_type->pa = this;

            }

            ~FuncDecl() override;
        };

        class GlobalMemoryDecl : public Base {
        public:
            Type        *type           = nullptr;
            Initializer *initializer    = nullptr;

            std::string to_string() const override;

            GlobalMemoryDecl(Type *type, Initializer *initializer) :
                type(type), initializer(initializer) {

                type->pa = this;
                initializer->pa = this;

            }

            ~GlobalMemoryDecl() override;
        };

        class GlobalSymbolDef : public GlobalStmt {
        public:
            Id                  *id     = nullptr;
            GlobalMemoryDecl    *decl   = nullptr;

            std::string to_string() const override;

            GlobalSymbolDef(Id *id, GlobalMemoryDecl *decl) :
                id(id), decl(decl) {

                id->pa = this;
                decl->pa = this;

            }

            ~GlobalSymbolDef() override;
        };

class Program : public Base {
public:
    std::vector<GlobalStmt*> global_stmts;

    std::string to_string() const override;

    std::string to_riscv() const override;

    Program(std::vector<GlobalStmt *> global_stmts) : 
        global_stmts(global_stmts) {

        for (auto global_stmt : global_stmts) global_stmt->pa = this;

    }

    ~Program() override;
};

}



#endif