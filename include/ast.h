#ifndef AST_H_
#define AST_H_

#include <iostream>


class BaseAst {
public:
    virtual ~BaseAst() = default;

    virtual void debug() const = 0;
};


class StmtAst : public BaseAst {
};

class ReturnStmtAst : public StmtAst {
public:
    int return_val = 0;

    ReturnStmtAst(int return_val) : return_val(return_val) {}

    void debug() const override;
};


class BlockAst : public BaseAst {
public:
    BaseAst *stmt;

    BlockAst(BaseAst *stmt) : stmt(stmt) {}

    void debug() const override;
};

class FuncTypeAst : public BaseAst {
};

class IntFuncTypeAst : public FuncTypeAst {
public:
    void debug() const override;
};

class FuncDefAst : public BaseAst {
public:
    BaseAst *func_type;
    std::string *id;
    BaseAst *block;

    FuncDefAst(BaseAst *func_type, std::string *id, BaseAst *block) :
        func_type(func_type), id(id), block(block) {}

    void debug() const override;
};

class CompUnitAst : public BaseAst {
public:
    BaseAst *func_def;

    CompUnitAst(BaseAst *func_def) : func_def(func_def) {}

    void debug() const override;
};

#endif