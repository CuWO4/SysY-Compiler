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

%type   <ast_val> comp_unit func_def func_type block stmt
%type   <int_val> number

%%

comp_unit
    : func_def {
        ast = new ast::CompUnit((ast::FuncDef *) $1);
    }
;

func_def
    : func_type TK_IDENT '(' ')' block {
        $$ = new ast::FuncDef(
            (ast::FuncType *) $1, 
            $2, 
            (ast::Block *) $5
        );
    }
;

func_type
    : TK_INT {
        $$ = new ast::IntFuncType();
    }
;

block
    : '{' stmt '}' {
        $$ = new ast::Block((ast::Stmt *) $2);
    }
;

stmt
    : TK_RETURN number ';' {
        $$ = new ast::Return($2);
    }
;

number
    : TK_INT_CONST
;

%%

void yyerror(ast::CompUnit *&ast, const char *s) {
    std::cerr << "error: " << s << std::endl;
}
