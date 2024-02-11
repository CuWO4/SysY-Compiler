#ifndef AST_H_
#define AST_H_

#include "koopa.h"

#include <iostream>

namespace ast {

class Base {
public:
    virtual ~Base() = default;

    virtual koopa::Base *to_koopa() const = 0;

    virtual void debug() const = 0;
};


namespace op {
    enum Op {
        LOGIC_OR, LOGIC_AND, EQ, NEQ, LT, GT, LEQ, GEQ,
        ADD, SUB, MUL, DIV, MOD, NEG, POS, NOT
    };
}

class Expr : public Base {
public:
    op::Op  op  = op::LOGIC_OR;
    Expr    *lv = nullptr;
    Expr    *rv = nullptr;

    Expr(op::Op op, Expr *lv, Expr *rv) : op(op), lv(lv), rv(rv) {}

    koopa::Base *to_koopa() const override;

    void debug() const override;
};

class Stmt : public Base {
};

    class Return : public Stmt {
    public:
        int return_val = 0;

        Return(int return_val) : return_val(return_val) {}

        koopa::Base *to_koopa() const override;

        void debug() const override;
    };

class Block : public Base {
public:
    Stmt *stmt = nullptr;

    Block(Stmt *stmt) : stmt(stmt) {}

    koopa::Base *to_koopa() const override;

    void debug() const override;

    ~Block() override;
};

class Type : public Base {
};

    class Int : public Type {
    public:
        koopa::Base *to_koopa() const override;

        void debug() const override;
    };

class FuncDef : public Base {
public:
    Type            *func_type  = nullptr;
    std::string     *id         = nullptr;
    Block           *block      = nullptr;

    FuncDef(Type *func_type, std::string *id, Block *block) :
        func_type(func_type), id(id), block(block) {}

    koopa::Base *to_koopa() const override;

    void debug() const override;

    ~FuncDef() override;
};

class CompUnit : public Base {
public:
    FuncDef *func_def = nullptr;

    CompUnit(FuncDef *func_def) : func_def(func_def) {}

    koopa::Base *to_koopa() const override;

    void debug() const override;

    ~CompUnit() override;
};

}

#endif