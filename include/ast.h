#ifndef AST_H_
#define AST_H_

#include <iostream>
#include <vector>
#include <functional>

namespace koopa {
    class Base;
    class ValueSaver;
}

namespace ast {

class NestingInfo {
public:
    bool need_suffix = true;
    int nesting_level = 0, nesting_count = 0;
    NestingInfo *pa = nullptr;

    NestingInfo(int nl = 0, int nc = 0, NestingInfo *pa = nullptr) :
        nesting_level(nl), nesting_count(nc), pa(pa) {}

    NestingInfo(bool need_suffix) : need_suffix(need_suffix) {}
};

class Base {
public:
    virtual koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const = 0;

    virtual std::string debug() const = 0;

    virtual ~Base() = default;
};

class Type : public Base {
};

    class Int : public Type {
    public:
        koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override;

        std::string debug() const override;
    };

class Id : public Base {
public:
    std::string *lit;

    Id(std::string *lit) : lit(lit) {}

    koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override;

    std::string debug() const override;

    ~Id() override;
};

namespace op {
    enum BinaryOp {
        LOGIC_OR, LOGIC_AND, EQ, NEQ, LT, GT, LEQ, GEQ,
        ADD, SUB, MUL, DIV, MOD,
    };

    enum UnaryOp {
        NEG, POS, NOT,
    };
}

class Expr : public Base {
};

    class BinaryExpr : public Expr {
    public:
        op::BinaryOp    op  = op::LOGIC_OR;
        Expr            *lv = nullptr;
        Expr            *rv = nullptr;

        BinaryExpr(op::BinaryOp op, Expr *lv, Expr* rv) :
            op(op), lv(lv), rv(rv) {}

        koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override;

        std::string debug() const override;

        ~BinaryExpr() override;
    };

    class UnaryExpr : public Expr {
    public:
        op::UnaryOp     op  = op::NEG;
        Expr            *lv = nullptr;

        UnaryExpr(op::UnaryOp op, Expr *lv) :
            op(op), lv(lv) {}

        koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override;

        std::string debug() const override;

        ~UnaryExpr() override;
    };

    class Number : public Expr {
    public:
        int val = 0;

        Number(int val) : val(val) {}

        koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override;

        std::string debug() const override;
    };

class Stmt : public Base {
};

    class ExprStmt : public Stmt {
    public:
        Expr *expr = nullptr;;

        ExprStmt(Expr *expr) : expr(expr) {}

        koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override;

        std::string debug() const override;

        ~ExprStmt() override;        
    };

    class VarDef : public Base{
    public:
        Id *id = nullptr;
        bool has_init = false;
        Expr *init = nullptr;

        VarDef(Id *id) : id(id) {}
        VarDef(Id *id, Expr *init) : id(id), init(init) { has_init = true; }

        koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override { return nullptr; }

        std::string debug() const override;

        ~VarDef();
    };
    class VarDecl : public Stmt {
    public:
        Type *type = nullptr;
        std::vector<VarDef *> var_defs = {};
        bool is_const = false;

        VarDecl(Type *type, std::vector<VarDef *> var_defs, 
                bool is_const = false) :
            type(type), var_defs(var_defs), is_const(is_const) {}

        koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override;

        std::string debug() const override;

        ~VarDecl() override;
    };

    class Assign : public Stmt {
    public:
        Id *id = nullptr;
        Expr *rval = nullptr;

        Assign(Id *id, Expr *rval) : id(id), rval(rval) {}

        koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override;

        std::string debug() const override;

        ~Assign() override;
    };

    class Return : public Stmt {
    public:
        Expr *ret_val = nullptr;

        Return(Expr *ret_val = nullptr) : ret_val(ret_val) {}

        koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override;

        std::string debug() const override;

        ~Return() override;
    };

    class Block : public Stmt {
    public:
        std::vector<Stmt *> stmts = {};

        NestingInfo *nesting_info;

        Block(std::vector<Stmt *> stmts, NestingInfo *nesting_info) 
            : stmts(stmts), nesting_info(nesting_info) {}

        koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override;

        std::string debug() const override;

        ~Block() override;
    };

class FuncDef : public Base {
public:
    Type            *func_type  = nullptr;
    std::string     *id         = nullptr;
    Block           *block      = nullptr;

    FuncDef(Type *func_type, std::string *id, Block *block) :
        func_type(func_type), id(id), block(block) {}

    koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override;

    std::string debug() const override;

    ~FuncDef() override;
};

class CompUnit : public Base {
public:
    FuncDef *func_def = nullptr;

    CompUnit(FuncDef *func_def) : func_def(func_def) {}

    koopa::Base *to_koopa(koopa::ValueSaver &value_saver, ast::NestingInfo *nesting_info) const override;

    std::string debug() const override;

    ~CompUnit() override;
};

}

#endif