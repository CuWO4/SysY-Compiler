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
%type   <int_val> expr number

%left TK_LOGIC_OR
%left TK_LOGIC_AND
%left TK_EQ TK_NEQ
%left '<' '>' TK_LEQ TK_GEQ
%left '+' '-'
%left '*' '/' '%'
%right TK_UNARY_OP

%%

comp_unit
    : func_def {
        ast = new ast::CompUnit((ast::FuncDef *) $1);
    }
;

func_def
    : func_type TK_IDENT '(' ')' block {
        $$ = new ast::FuncDef(
            (ast::Type *) $1, 
            $2, 
            (ast::Block *) $5
        );
    }
;

func_type
    : TK_INT {
        $$ = new ast::Int();
    }
;

block
    : '{' stmt '}' {
        $$ = new ast::Block((ast::Stmt *) $2);
    }
;

stmt
    : TK_RETURN expr ';' {
        $$ = new ast::Return($2);
    }
;

expr
    : '(' expr ')'                  {}
    /* TODO short circuit evaluation */
    | expr TK_LOGIC_OR expr         {}
    | expr TK_LOGIC_AND expr        {}
    | expr TK_EQ expr               {}
    | expr TK_NEQ expr              {}
    | expr '<' expr                 {}
    | expr '>' expr                 {}
    | expr TK_LEQ expr              {}
    | expr TK_GEQ expr              {}
    | expr '+' expr                 {}
    | expr '-' expr                 {}
    | expr '*' expr                 {}
    | expr '/' expr                 {}
    | expr '%' expr                 {}
    | '-' expr %prec TK_UNARY_OP    {}
    | '+' expr %prec TK_UNARY_OP    {}
    | '!' expr %prec TK_UNARY_OP    {}
    | number                        {}
;

number 
    : TK_INT_CONST
;

%%

void yyerror(ast::CompUnit *&ast, const char *s) {
    std::cerr << "error: " << s << std::endl;
}
