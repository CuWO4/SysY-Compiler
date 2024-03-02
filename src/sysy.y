%code requires {
    #include <iostream>
    #include <memory>
    #include <string>

    #include "../include/ast.h"
    #include "../include/nesting_info.h"
    #include "../include/trans.h"

    int yylex();
    void yyerror(ast::CompUnit *&ast, const char *s);
}

%{
    #include "../include/ast.h"

    int cur_nesting_level = 0;
    int cur_nesting_count[4096] = { 0 };
    NestingInfo *cur_nesting_info = nullptr;
%}

%parse-param    { ast::CompUnit *&ast }

%union {
    std::string    *str_val;
    int             int_val;
    ast::Base      *ast_val;
    std::vector<ast::Stmt *>    *stmt_vec_val;
    std::vector<ast::VarDef *>  *var_def_vec_val;
	}

%token TK_INT TK_RETURN TK_CONST
%token <str_val> TK_IDENT
%token <int_val> TK_INT_CONST

%type   <ast_val> comp_unit func_def type block block_item decl var_def stmt expr
%type   <stmt_vec_val> block_items 
%type   <var_def_vec_val> var_defs
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
    : type TK_IDENT '(' ')' block {
        $$ = new ast::FuncDef(
            static_cast<ast::Type *>($1),
            $2,
            static_cast<ast::Block *>($5)
        );
    }
;

type
    : TK_INT {
        $$ = new ast::Int();
    }
;

block
    : block_start block_items '}' {
        $$ = new ast::Block(*$2, cur_nesting_info);

        cur_nesting_level--;

        cur_nesting_info = cur_nesting_info->pa;
    }
;

block_start : '{' {
    auto new_nesting_info = new NestingInfo(cur_nesting_level, cur_nesting_count[cur_nesting_level], cur_nesting_info);
    cur_nesting_info = new_nesting_info;

    cur_nesting_count[cur_nesting_level]++;
    cur_nesting_level++;
}

block_items
    : block_items block_item {
        if ($2 != nullptr) $1->push_back(static_cast<ast::Stmt *>($2));
        $$ = $1;
    }
    | {
        $$ = new std::vector<ast::Stmt *>{};
    }
;

block_item
    : decl
    | stmt
    | expr ';' {
        $$ = new ast::ExprStmt(static_cast<ast::Expr *>($1));
    }
    | block
    | ';'           { $$ = nullptr; }
    | error ';'     { yyerrok; }
;

decl
    : type var_defs ';' {
        $$ = new ast::VarDecl(static_cast<ast::Type *>($1), *$2);
    }
    | TK_CONST type var_defs ';' {
        $$ = new ast::VarDecl(static_cast<ast::Type *>($2), *$3, true);
    }
;

var_defs
    : var_defs ',' var_def {
        $1->push_back(static_cast<ast::VarDef *>($3));
        $$ = $1;
    }
    | var_def {
        $$ = new std::vector<ast::VarDef *>{ static_cast<ast::VarDef *>($1) };
    }
;

var_def
    : TK_IDENT '=' expr {
        $$ = new ast::VarDef(new ast::Id($1), static_cast<ast::Expr *>($3));
    }
    | TK_IDENT {
        $$ = new ast::VarDef(new ast::Id($1));
    }
;

stmt
    : TK_IDENT '=' expr ';' {
        $$ = new ast::Assign(new ast::Id($1), static_cast<ast::Expr *>($3));
    }
    | TK_RETURN expr ';' {
        $$ = new ast::Return(static_cast<ast::Expr *>($2));
    }
    | TK_RETURN ';' {
        $$ = new ast::Return();
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
    | '-' expr %prec PREC_UNARY_OP  {
		$$ = new ast::UnaryExpr(ast::op::NEG, static_cast<ast::Expr *>($2));
	}
    | '+' expr %prec PREC_UNARY_OP  {
		$$ = new ast::UnaryExpr(ast::op::POS, static_cast<ast::Expr *>($2));
	}
    | '!' expr %prec PREC_UNARY_OP  {
		$$ = new ast::UnaryExpr(ast::op::NOT, static_cast<ast::Expr *>($2));
	}
    | number                        {
		$$ = new ast::Number($1);
	}
    | TK_IDENT                      {
        $$ = new ast::Id(new std::string(*$1));
    }
;

number
    : TK_INT_CONST
;

%%

void yyerror(ast::CompUnit *&ast, const char *s) {
    throw std::string(s);
}
