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

%type   <ast_val> comp_unit func_def func_type block stmt expr
%type   <int_val> number

%left TK_LOGIC_OR
%left TK_LOGIC_AND
%left TK_EQ TK_NEQ
%left '<' '>' TK_LEQ TK_GEQ
%left '+' '-'
%left '*' '/' '%'
%right PREC_UNARY_OP

%%

comp_unit
    : func_def {
        ast = new ast::CompUnit(static_cast<ast::FuncDef *>($1));
    }
;

func_def
    : func_type TK_IDENT '(' ')' block {
        $$ = new ast::FuncDef(
            static_cast<ast::Type *>($1), 
            $2, 
            static_cast<ast::Block *>($5)
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
        $$ = new ast::Block(static_cast<ast::Stmt *>($2));
    }
;

stmt
    : TK_RETURN expr ';' {
        $$ = new ast::Return(static_cast<ast::Expr *>($2));
    }
;

expr
    : '(' expr ')'                  { $$ = $2; }
    | expr TK_LOGIC_OR expr         {
		$$ = new ast::BinaryExpr(ast::op::LOGIC_OR, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | expr TK_LOGIC_AND expr        {
		$$ = new ast::BinaryExpr(ast::op::LOGIC_AND, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | expr TK_EQ expr               {
		$$ = new ast::BinaryExpr(ast::op::EQ, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | expr TK_NEQ expr              {
		$$ = new ast::BinaryExpr(ast::op::NEQ, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | expr '<' expr                 {
		$$ = new ast::BinaryExpr(ast::op::LT, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | expr '>' expr                 {
		$$ = new ast::BinaryExpr(ast::op::GT, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | expr TK_LEQ expr              {
		$$ = new ast::BinaryExpr(ast::op::LEQ, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | expr TK_GEQ expr              {
		$$ = new ast::BinaryExpr(ast::op::GEQ, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | expr '+' expr                 {
		$$ = new ast::BinaryExpr(ast::op::ADD, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | expr '-' expr                 {
		$$ = new ast::BinaryExpr(ast::op::SUB, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | expr '*' expr                 {
		$$ = new ast::BinaryExpr(ast::op::MUL, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | expr '/' expr                 {
		$$ = new ast::BinaryExpr(ast::op::DIV, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | expr '%' expr                 {
		$$ = new ast::BinaryExpr(ast::op::MOD, static_cast<ast::Expr *>($1), static_cast<ast::Expr *>($3));
}
    | '-' expr %prec PREC_UNARY_OP    {
		$$ = new ast::UnaryExpr(ast::op::NEG, static_cast<ast::Expr *>($2));
}
    | '+' expr %prec PREC_UNARY_OP    {
		$$ = new ast::UnaryExpr(ast::op::POS, static_cast<ast::Expr *>($2));
}
    | '!' expr %prec PREC_UNARY_OP    {
		$$ = new ast::UnaryExpr(ast::op::NOT, static_cast<ast::Expr *>($2));
}
    | number                        {
		$$ = new ast::Number($1);
}
;

number 
    : TK_INT_CONST
;

%%

void yyerror(ast::CompUnit *&ast, const char *s) {
    std::cerr << "error: " << s << std::endl;
}
