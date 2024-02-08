%code requires {
    #include <iostream>
    #include <memory>
    #include <string>

    #include "../include/ast.h"

    int yylex();
    void yyerror(CompUnitAst *&ast, const char *s);
}

%parse-param    { CompUnitAst *&ast }

%union {
    std::string    *str_val;
    int             int_val;
    BaseAst        *ast_val;
}

%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST

%type   <ast_val> CompUnit FuncDef FuncType Block Stmt
%type   <int_val> Number

%%

CompUnit
    : FuncDef {
        ast = new CompUnitAst($1);
    }
;

FuncDef
    : FuncType IDENT '(' ')' Block {
        $$ = new FuncDefAst($1, $2, $5);
    }
;

FuncType
    : INT {
        $$ = new IntFuncTypeAst();
    }
;

Block
    : '{' Stmt '}' {
        $$ = new BlockAst($2);
    }
;

Stmt
    : RETURN Number ';' {
        $$ = new ReturnStmtAst($2);
    }
;

Number
    : INT_CONST
;

%%

void yyerror(CompUnitAst *&ast, const char *s) {
    std::cerr << "error: " << s << std::endl;
}
