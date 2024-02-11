#ifndef AST_H_
#define AST_H_

#include "koopa.h"

#include <iostream>
#include <functional>

namespace ast {

class Base {
public:
    virtual ~Base() = default;

    virtual koopa::Base *to_koopa() const = 0;

    virtual std::string debug() const = 0;
};


namespace op {
    enum BinaryOp {
        LOGIC_OR, LOGIC_AND, EQ, NEQ, LT, GT, LEQ, GEQ,
        ADD, SUB, MUL, DIV, MOD,
    };

    static std::function<int(int, int)> BinaryOpFunc[] = {
        [](int a, int b) { return a || b; },
        [](int a, int b) { return a && b; },
        [](int a, int b) { return a == b; },
        [](int a, int b) { return a != b; },
        [](int a, int b) { return a < b; },
        [](int a, int b) { return a > b; },
        [](int a, int b) { return a <= b; },
        [](int a, int b) { return a >= b; },
        [](int a, int b) { return a + b; },
        [](int a, int b) { return a - b; },
        [](int a, int b) { return a * b; },
        [](int a, int b) { return a / b; },
        [](int a, int b) { return a % b; },
    };

    enum UnaryOp {
        NEG, POS, NOT,
    };

    static std::function<int(int)> UnaryOpFunc[] = {
        [](int a) { return -a; },
        [](int a) { return a; },
        [](int a) { return !a; },
    };
}

class Expr : public Base {
public:
    int val = 0;
};

    class BinaryExpr : public Expr {
    public:
        op::BinaryOp    op  = op::LOGIC_OR;
        Expr            *lv = nullptr;
        Expr            *rv = nullptr;

        BinaryExpr(op::BinaryOp op, Expr *lv, Expr* rv) :
            op(op), lv(lv), rv(rv) {
            val = op::BinaryOpFunc[op](lv->val, rv->val);
        }

        koopa::Base *to_koopa() const override;

        std::string debug() const override;

        ~BinaryExpr() override;
    };

    class UnaryExpr : public Expr {
    public:
        op::UnaryOp     op  = op::NEG;
        Expr            *lv = nullptr;

        UnaryExpr(op::UnaryOp op, Expr *lv) :
            op(op), lv(lv) {
            val = op::UnaryOpFunc[op](lv->val);
        }

        koopa::Base *to_koopa() const override;

        std::string debug() const override;

        ~UnaryExpr() override;
    };

    class Number : public Expr {
    public:
        Number(int val) { this->val = val; }

        koopa::Base *to_koopa() const override;

        std::string debug() const override;
    };

class Stmt : public Base {
};

    class Return : public Stmt {
    public:
        Expr *ret_val = nullptr;

        Return(Expr *ret_val = nullptr) : ret_val(ret_val) {}

        koopa::Base *to_koopa() const override;

        std::string debug() const override;
    };

class Block : public Base {
public:
    Stmt *stmt = nullptr;

    Block(Stmt *stmt) : stmt(stmt) {}

    koopa::Base *to_koopa() const override;

    std::string debug() const override;

    ~Block() override;
};

class Type : public Base {
};

    class Int : public Type {
    public:
        koopa::Base *to_koopa() const override;

        std::string debug() const override;
    };

class FuncDef : public Base {
public:
    Type            *func_type  = nullptr;
    std::string     *id         = nullptr;
    Block           *block      = nullptr;

    FuncDef(Type *func_type, std::string *id, Block *block) :
        func_type(func_type), id(id), block(block) {}

    koopa::Base *to_koopa() const override;

    std::string debug() const override;

    ~FuncDef() override;
};

class CompUnit : public Base {
public:
    FuncDef *func_def = nullptr;

    CompUnit(FuncDef *func_def) : func_def(func_def) {}

    koopa::Base *to_koopa() const override;

    std::string debug() const override;

    ~CompUnit() override;
};

}

#endif