%code requires {
    #include <iostream>
    #include <memory>
    #include <string>

    #include "../include/ast.h"

    int yylex();
    void yyerror(ast::CompUnit *&ast, const char *s);
}

%parse-param    { ast::CompUnit *&ast }

%union {
    std::string    *str_val;
    int             int_val;
    ast::Base      *ast_val;
}

%token TK_INT TK_RETURN
%token <str_val> TK_IDENT
%token <int_val> TK_INT_CONST

%type   <ast_val> CompUnit FuncDef FuncType Block Stmt
%type   <int_val> Number

%%

CompUnit
    : FuncDef {
        ast = new ast::CompUnit($1);
    }
;

FuncDef
    : FuncType TK_IDENT '(' ')' Block {
        $$ = new ast::FuncDef($1, $2, $5);
    }
;

FuncType
    : TK_INT {
        $$ = new ast::IntFuncType();
    }
;

Block
    : '{' Stmt '}' {
        $$ = new ast::Block($2);
    }
;

Stmt
    : TK_RETURN Number ';' {
        $$ = new ast::ReturnStmt($2);
    }
;

Number
    : TK_INT_CONST
;

%%

void yyerror(ast::CompUnit *&ast, const char *s) {
    std::cerr << "error: " << s << std::endl;
}
