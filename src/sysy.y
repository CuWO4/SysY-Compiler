%code requires {
    #include <iostream>
    #include <memory>
    #include <string>
    #include <tuple>

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

    ast::FuncDef   *ast_func_def_val;
    ast::Type      *ast_type_val;
    ast::Block     *ast_block_val;
    ast::Stmt      *ast_stmt_val;
    ast::GlobalStmt    *ast_global_stmt_val;
    ast::Expr      *ast_expr_val;
    ast::VarDef    *ast_var_def_val;
    std::vector<ast::Stmt *>    *ast_stmt_vec_val;
    std::vector<ast::GlobalStmt *>    *ast_global_stmt_vec_val;
    std::vector<ast::VarDef *>  *ast_var_def_vec_val;
    std::tuple<ast::Type *, ast::Id *>    *ast_func_param_val;
    std::vector<std::tuple<ast::Type *, ast::Id *> *>  *ast_func_params_val;
	}

%token TK_INT TK_RETURN TK_CONST TK_IF TK_ELSE TK_WHILE TK_FOR TK_CONTINUE TK_BREAK
%token <str_val> TK_IDENT
%token <int_val> TK_INT_CONST

%type	<ast_func_def_val> func_def
%type	<ast_type_val> type
%type	<ast_block_val> block
%type	<ast_global_stmt_val> comp_unit_item
%type	<ast_stmt_val> block_item 
%type   <ast_stmt_val> clause if_clause while_clause for_clause
%type   <ast_stmt_val> stmt decl_stmt return_stmt continue_stmt break_stmt block_stmt empty_stmt for_init_stmt for_iter_stmt
%type	<ast_expr_val> expr for_cond_expr
%type	<ast_var_def_val> var_def
%type   <ast_global_stmt_vec_val> comp_unit_items
%type   <ast_stmt_vec_val> block_items 
%type   <ast_var_def_vec_val> var_defs
%type   <ast_func_param_val> func_def_param 
%type   <ast_func_params_val> func_def_params 
%type   <int_val> number

%left ','
%right '='
%left TK_LOGIC_OR
%left TK_LOGIC_AND
%left TK_EQ TK_NEQ
%left '<' '>' TK_LEQ TK_GEQ
%left '+' '-'
%left '*' '/' '%'
%right PREC_UNARY_OP

// resolve the dangling-else by give a precedence 
%nonassoc PREC_IF
%nonassoc TK_ELSE

%%

comp_unit
    : comp_unit_items {
        ast = new ast::CompUnit(*$1);
    }
;

comp_unit_items
    : comp_unit_items comp_unit_item {
        $1->push_back($2);
        $$ = $1;
    }
    | {
        $$ = new std::vector<ast::GlobalStmt *>();
    }
;

comp_unit_item
    : func_def { $$ = $1; }
;

func_def
    : type TK_IDENT block_start '(' func_def_params ')' block block_end {
        $$ = new ast::FuncDef($1, $2, *$5, $7);
    }
    | type TK_IDENT block_start '(' ')' block block_end {
        $$ = new ast::FuncDef($1, $2, {}, $6);
    }
;

func_def_params
    : func_def_params ',' func_def_param {
        $1->push_back($3);
        $$ = $1;
    }
    | func_def_param {
        $$ = new std::vector<std::tuple<ast::Type *, ast::Id *> *> { $1 };
    }
;

func_def_param
    : type TK_IDENT {
        $$ = new std::tuple<ast::Type *, ast::Id *>($1, new ast::Id($2, cur_nesting_info));
    }
;

type
    : TK_INT {
        $$ = new ast::Int();
    }
;

block
    : block_start '{' block_items '}' block_end {
        $$ = new ast::Block(*$3);
    }
;

block_items
    : block_items block_item {
        $1->push_back($2);
        $$ = $1;
    }
    | {
        $$ = new std::vector<ast::Stmt *>{};
    }
;

block_item
    : stmt ';'
    | clause
    | block         { $$ = $1; }
    | error ';'     { yyerrok; }
;

clause
    : if_clause
    | while_clause
    | for_clause
;

if_clause
    : TK_IF '(' expr ')' block_stmt %prec PREC_IF {
        $$ =new ast::If($3, $5);
    }
    | TK_IF '(' expr ')' block_stmt TK_ELSE block_stmt {
        $$ = new ast::If($3, $5, $7);
    }
;

while_clause
    : TK_WHILE '(' expr ')' block_stmt {
        $$ = new ast::While($3, $5);
    }
;

for_clause
    : TK_FOR block_start '(' for_init_stmt ';' for_cond_expr ';' for_iter_stmt ')' block_stmt block_end {
        $$ = new ast::For($4, $6, $8, $10);
    }
;

for_init_stmt
    : decl_stmt
    | expr      { $$ = $1; }
    | empty_stmt
;

for_cond_expr
    : expr
    |           { $$ = new ast::Number(1); }
;

for_iter_stmt
    : expr      { $$ = $1; }
    | empty_stmt
;

block_stmt
    : block_start stmt ';' block_end    { $$ = $2; }
    | clause
    | block                             { $$ = $1; }  
;

stmt
    : decl_stmt
    | return_stmt
    | continue_stmt
    | break_stmt
    | expr          { $$ = $1; }
    | empty_stmt
;

decl_stmt
    : type var_defs {
        $$ = new ast::VarDecl($1, *$2);
    }
    | TK_CONST type var_defs {
        $$ = new ast::VarDecl($2, *$3, true);
    }
;

var_defs
    : var_defs ',' var_def {
        $1->push_back($3);
        $$ = $1;
    }
    | var_def {
        $$ = new std::vector<ast::VarDef *>{ $1 };
    }
;

var_def
    : TK_IDENT '=' expr {
        $$ = new ast::VarDef(new ast::Id($1, cur_nesting_info), $3);
    }
    | TK_IDENT {
        $$ = new ast::VarDef(new ast::Id($1, cur_nesting_info));
    }
;

return_stmt
    : TK_RETURN expr  {
        $$ = new ast::Return($2);
    }
    | TK_RETURN  {
        $$ = new ast::Return();
    }
;

continue_stmt
    : TK_CONTINUE  {
        $$ = new ast::Continue();
    }
;

break_stmt
    : TK_BREAK  {
        $$ = new ast::Break();
    }
;

empty_stmt
    :        { $$ = new ast::Block({}); }
;

expr
    : '(' expr ')'                  { $$ = $2; }
    | expr TK_LOGIC_OR expr         {
		$$ = new ast::BinaryExpr(ast::op::LOGIC_OR, $1, $3);
	}
    | expr TK_LOGIC_AND expr        {
		$$ = new ast::BinaryExpr(ast::op::LOGIC_AND, $1, $3);
	}
    | expr TK_EQ expr               {
		$$ = new ast::BinaryExpr(ast::op::EQ, $1, $3);
	}
    | expr TK_NEQ expr              {
		$$ = new ast::BinaryExpr(ast::op::NEQ, $1, $3);
	}
    | expr '<' expr                 {
		$$ = new ast::BinaryExpr(ast::op::LT, $1, $3);
	}
    | expr '>' expr                 {
		$$ = new ast::BinaryExpr(ast::op::GT, $1, $3);
	}
    | expr TK_LEQ expr              {
		$$ = new ast::BinaryExpr(ast::op::LEQ, $1, $3);
	}
    | expr TK_GEQ expr              {
		$$ = new ast::BinaryExpr(ast::op::GEQ, $1, $3);
	}
    | expr '+' expr                 {
		$$ = new ast::BinaryExpr(ast::op::ADD, $1, $3);
	}
    | expr '-' expr                 {
		$$ = new ast::BinaryExpr(ast::op::SUB, $1, $3);
	}
    | expr '*' expr                 {
		$$ = new ast::BinaryExpr(ast::op::MUL, $1, $3);
	}
    | expr '/' expr                 {
		$$ = new ast::BinaryExpr(ast::op::DIV, $1, $3);
	}
    | expr '%' expr                 {
		$$ = new ast::BinaryExpr(ast::op::MOD, $1, $3);
	}
    | expr '=' expr                 {
		$$ = new ast::BinaryExpr(ast::op::ASSIGN, $1, $3);
	}
    | expr ',' expr                 {
		$$ = new ast::BinaryExpr(ast::op::COMMA, $1, $3);
	}
    | '-' expr %prec PREC_UNARY_OP  {
		$$ = new ast::UnaryExpr(ast::op::NEG, $2);
	}
    | '+' expr %prec PREC_UNARY_OP  {
		$$ = new ast::UnaryExpr(ast::op::POS, $2);
	}
    | '!' expr %prec PREC_UNARY_OP  {
		$$ = new ast::UnaryExpr(ast::op::NOT, $2);
	}
    | number                        {
		$$ = new ast::Number($1);
	}
    | TK_IDENT                      {
        $$ = new ast::Id(new std::string(*$1), cur_nesting_info);
    }
;

number
    : TK_INT_CONST
;

block_start : {
    cur_nesting_level++;

    auto new_nesting_info = new NestingInfo(
        cur_nesting_level, 
        cur_nesting_count[cur_nesting_level],
        cur_nesting_info
    );
    cur_nesting_info = new_nesting_info;
}

block_end : {
    cur_nesting_count[cur_nesting_level]++;
    cur_nesting_level--;

    cur_nesting_info = cur_nesting_info->pa;
}

%%

void yyerror(ast::CompUnit *&ast, const char *s) {
    throw std::string(s);
}
