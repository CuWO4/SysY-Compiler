#ifndef AST_H_
#define AST_H_

#include <iostream>

namespace ast {

class Base {
public:
    virtual ~Base() = default;

    virtual void debug() const = 0;
};


class Stmt : public Base {
};

class ReturnStmt : public Stmt {
public:
    int return_val = 0;

    ReturnStmt(int return_val) : return_val(return_val) {}

    void debug() const override;
};

class Block : public Base {
public:
    Base *stmt;

    Block(Base *stmt) : stmt(stmt) {}

    void debug() const override;

    ~Block() override;
};

class FuncType : public Base {
};

class IntFuncType : public FuncType {
public:
    void debug() const override;
};

class FuncDef : public Base {
public:
    Base *func_type;
    std::string *id;
    Base *block;

    FuncDef(Base *func_type, std::string *id, Base *block) :
        func_type(func_type), id(id), block(block) {}

    void debug() const override;

    ~FuncDef() override;
};

class CompUnit : public Base {
public:
    Base *func_def;

    CompUnit(Base *func_def) : func_def(func_def) {}

    void debug() const override;

    ~CompUnit() override;
};

}

#endif