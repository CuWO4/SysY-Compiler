%code requires {
    #include <iostream>
    #include <memory>
    #include <string>
    #include <tuple>

    #include "../include/ast.h"
    #include "../include/nesting_info.h"

    int yylex();
    void yyerror(ast::CompUnit *&ast, const char *s);
}

%{
    #include "../include/ast.h"

    int cur_nesting_level = 0;
    int cur_nesting_count[4096] = { 0 };
    NestingInfo *cur_nesting_info = new NestingInfo();
%}

%parse-param    { ast::CompUnit *&ast }

%union {
    std::string    *str_val;
    int             int_val;

    ast::Id        *ast_id_val;
    ast::Type      *ast_type_val;
    ast::Block     *ast_block_val;
    ast::Stmt      *ast_stmt_val;
    ast::GlobalStmt    *ast_global_stmt_val;
    ast::Expr      *ast_expr_val;
    ast::VarDef    *ast_var_def_val;
    ast::GlobalVarDef  *ast_global_var_def_val;
    std::vector<ast::Expr *>               *ast_expr_vec_val;
    std::vector<ast::Stmt *>               *ast_stmt_vec_val;
    std::vector<ast::GlobalStmt *>         *ast_global_stmt_vec_val;
    std::vector<ast::VarDef *>             *ast_var_def_vec_val;
    std::vector<ast::GlobalVarDef *>       *ast_global_var_def_vec_val;
    std::tuple<ast::Type *, ast::Id *>     *ast_func_param_val;
    std::vector<std::tuple<ast::Type *, ast::Id *> *>  *ast_func_params_val;
	}

%token TK_INT TK_VOID TK_RETURN TK_CONST TK_IF TK_ELSE TK_WHILE TK_FOR TK_CONTINUE TK_BREAK
%token <str_val> TK_IDENT
%token <int_val> TK_INT_CONST

%type   <ast_id_val> id
%type	<ast_type_val> type
%type	<ast_block_val> block
%type	<ast_global_stmt_val> comp_unit_item func_def global_var_decl
%type	<ast_stmt_val> block_item 
%type   <ast_stmt_val> clause if_clause while_clause for_clause
%type   <ast_stmt_val> stmt decl_stmt return_stmt continue_stmt break_stmt block_stmt empty_stmt for_init_stmt for_iter_stmt
%type	<ast_expr_val> expr no_comma_expr func_call for_cond_expr
%type	<ast_var_def_val> var_def
%type   <ast_global_var_def_val> global_var_def
%type   <ast_global_stmt_vec_val> comp_unit_items
%type   <ast_expr_vec_val> func_call_params
%type   <ast_stmt_vec_val> block_items 
%type   <ast_var_def_vec_val> var_defs
%type   <ast_global_var_def_vec_val> global_var_defs
%type   <ast_func_param_val> func_def_param 
%type   <ast_func_params_val> func_def_params 
%type   <int_val> number

// resolve the dangling-else by give a precedence 
%nonassoc PREC_IF
%nonassoc TK_ELSE

%left ','
%right '='
%left TK_LOGIC_OR
%left TK_LOGIC_AND
%left TK_EQ TK_NEQ
%left '<' '>' TK_LEQ TK_GEQ
%left '+' '-'
%left '*' '/' '%'
%right PREC_UNARY_OP

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
    : func_def
    | global_var_decl ';'       { $$ = $1; }
;

global_var_decl
    : type global_var_defs {
        $$ = new ast::GlobalVarDecl($1, *$2);
    }
    | TK_CONST type global_var_defs {
        $$ = new ast::GlobalVarDecl($2, *$3, ast::decl_type::ConstDecl);
    }
;

global_var_defs
    : global_var_defs ',' global_var_def {
        $1->push_back($3);
        $$ = $1;
    }
    | global_var_def {
        $$ = new std::vector<ast::GlobalVarDef *>{ $1 };
    }
;

global_var_def
    : id '=' no_comma_expr {
        $$ = new ast::GlobalVarDef($1, $3);
    }
    | id { 
        $$ = new ast::GlobalVarDef($1);
    }
;

func_def
    : type id block_start '(' func_def_params ')' block block_end {
        $$ = new ast::FuncDef($1, $2, *$5, $7);
    }
    | type id block_start '(' ')' block block_end {
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
    : type id {
        $$ = new std::tuple<ast::Type *, ast::Id *>($1, $2);
    }
;

type
    : TK_INT {
        $$ = new ast::Int();
    }
    | TK_VOID {
        $$ = new ast::Void();
    }
;

block
    : '{' block_items '}' {
        $$ = new ast::Block(*$2);
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
    | block_start block block_end   { $$ = $2; }
    | error ';'                     { yyerrok; }
;

clause
    : if_clause
    | while_clause
    | for_clause
;

if_clause
    : TK_IF '(' expr ')' block_start block_stmt block_end %prec PREC_IF {
        $$ =new ast::If($3, $6);
    }
    | TK_IF '(' expr ')' block_start block_stmt block_end TK_ELSE block_start block_stmt block_end {
        $$ = new ast::If($3, $6, $10);
    }
;

while_clause
    : TK_WHILE '(' expr ')' block_start block_stmt block_end {
        $$ = new ast::While($3, $6);
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
    : stmt ';'    { $$ = $1; }
    | clause
    | block       { $$ = $1; }  
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
        $$ = new ast::VarDecl($2, *$3, ast::decl_type::ConstDecl);
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
    : id '=' no_comma_expr {
        $$ = new ast::VarDef($1, $3);
    }
    | id { 
        $$ = new ast::VarDef($1);
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
    : no_comma_expr
    | no_comma_expr ',' no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::COMMA, $1, $3);
	}
;

no_comma_expr
    : '(' no_comma_expr ')' { $$ = $2; }
    | no_comma_expr TK_LOGIC_OR no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::LOGIC_OR, $1, $3);
	}
    | no_comma_expr TK_LOGIC_AND no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::LOGIC_AND, $1, $3);
	}
    | no_comma_expr TK_EQ no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::EQ, $1, $3);
	}
    | no_comma_expr TK_NEQ no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::NEQ, $1, $3);
	}
    | no_comma_expr '<' no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::LT, $1, $3);
	}
    | no_comma_expr '>' no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::GT, $1, $3);
	}
    | no_comma_expr TK_LEQ no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::LEQ, $1, $3);
	}
    | no_comma_expr TK_GEQ no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::GEQ, $1, $3);
	}
    | no_comma_expr '+' no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::ADD, $1, $3);
	}
    | no_comma_expr '-' no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::SUB, $1, $3);
	}
    | no_comma_expr '*' no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::MUL, $1, $3);
	}
    | no_comma_expr '/' no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::DIV, $1, $3);
	}
    | no_comma_expr '%' no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::MOD, $1, $3);
	}
    | no_comma_expr '=' no_comma_expr {
		$$ = new ast::BinaryExpr(ast::op::ASSIGN, $1, $3);
	}
    | '-' no_comma_expr %prec PREC_UNARY_OP  {
		$$ = new ast::UnaryExpr(ast::op::NEG, $2);
	}
    | '+' no_comma_expr %prec PREC_UNARY_OP  {
		$$ = new ast::UnaryExpr(ast::op::POS, $2);
	}
    | '!' no_comma_expr %prec PREC_UNARY_OP  {
		$$ = new ast::UnaryExpr(ast::op::NOT, $2);
	}
    | func_call
    | id { $$ = $1; }
    | number                        {
		$$ = new ast::Number($1);
	}
;

func_call
    : id '(' ')' {
        $$ = new ast::FuncCall($1, {});
    }
    | id '(' func_call_params ')' {
        $$ = new ast::FuncCall($1, *$3);
    }
;

func_call_params
    : func_call_params ',' no_comma_expr {
        $1->push_back($3);
        $$ = $1;
    }
    | no_comma_expr {
        $$ = new std::vector<ast::Expr *>{ $1 };
    }
;

id
    : TK_IDENT {
        $$ = new ast::Id(new std::string(*$1), cur_nesting_info);
    }
;

number
    : TK_INT_CONST
;

block_start: {
    cur_nesting_level++;

    auto new_nesting_info = new NestingInfo(
        cur_nesting_level, 
        cur_nesting_count[cur_nesting_level],
        cur_nesting_info
    );
    cur_nesting_info = new_nesting_info;
}

block_end: {
    cur_nesting_count[cur_nesting_level]++;
    cur_nesting_level--;

    cur_nesting_info = cur_nesting_info->pa;
}

%%

void yyerror(ast::CompUnit *&ast, const char *s) {
    throw std::string(s);
}
