%code requires {
    #include <iostream>
    #include <memory>
    #include <string>
    #include <tuple>

    #include "../include/ast.h"
    #include "../include/nesting_info.h"
    #include "../include/parser.hpp"

    int yylex();
    void yyerror(ast::CompUnit*& ast, const char* s);
}

%{
    #include "../include/ast.h"

    int cur_nesting_level = 0;
    int cur_nesting_count[4096] = { 0 };
    NestingInfo* cur_nesting_info = new NestingInfo;
%}

%parse-param    { ast::CompUnit*& ast }

%union {
    int Int;
    std::string* String;

    parser::VarDefManager* ParserVarDefManager;
    std::vector<parser::VarDefManager*>* ParserVarDefManagerVector;

    ast::Id* AstId;
    std::vector<ast::Expr*>* AstExprVector;

    ast::Type* AstType;

    ast::Expr* AstExpr;

    ast::Initializer* AstInitializer;
    std::vector<ast::Initializer*>* AstInitializerVector;

    ast::Stmt* AstStmt;
    ast::GlobalStmt* AstGlobalStmt;
    std::vector<ast::Stmt*>* AstStmtVector;
    std::vector<ast::GlobalStmt*>* AstGlobalStmtVector;

    ast::Block* AstBlock;

    std::tuple<ast::Type *, ast::Id*>* TypeIdTuple;
    std::vector<std::tuple<ast::Type *, ast::Id*>*>* TypeIdTupleVector;
}

%token TK_INT TK_VOID TK_RETURN TK_CONST TK_IF TK_ELSE TK_WHILE TK_FOR TK_CONTINUE TK_BREAK

%token  <Int> TK_INT_CONST
%token  <String> TK_IDENT

%type   <AstGlobalStmtVector> comp_unit_items
%type	<AstGlobalStmt> comp_unit_item 

%type   <AstGlobalStmt> func_def
%type   <TypeIdTuple> func_def_param
%type   <TypeIdTupleVector> func_def_params

%type	<AstBlock> block
%type	<AstStmt> block_item
%type   <AstStmtVector> block_items

%type   <AstStmt> stmt

%type   <AstStmt> return_stmt continue_stmt break_stmt block_stmt empty_stmt

%type   <AstStmt> decl_stmt
%type   <AstGlobalStmt> global_var_decl

%type   <ParserVarDefManager> var_def
%type	<ParserVarDefManagerVector> var_defs
%type   <AstExprVector> var_def_trace

%type   <AstInitializer> initializer
%type   <AstInitializerVector> initializers

%type   <AstStmt> clause 

%type   <AstStmt> if_clause while_clause 

%type   <AstStmt> for_clause 
%type   <AstExpr> for_cond_expr
%type   <AstStmt> for_init_stmt for_iter_stmt

%type	<AstExpr> expr 
%type   <AstExpr> no_comma_expr

%type   <AstExpr> func_call
%type   <AstExprVector> func_call_params

%type   <AstExprVector> indexing_trace

%type   <Int> number
%type   <AstId> id
%type	<AstType> type


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
%left '['

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
        $$ = new std::vector<ast::GlobalStmt*>;
    }
;

comp_unit_item
    : func_def
    | global_var_decl ';'       { $$ = $1; }
;

global_var_decl
    : type var_defs {
        auto var_defs = std::vector<ast::GlobalVarDef*>();
        var_defs.reserve($2->size());

        for (auto manager: *$2) {
            var_defs.push_back(
                manager->to_ast_var_def<ast::VolatileGlobalVarDef>($1)
            );
        }

        $$ = new ast::GlobalVarDecl(var_defs);
    }
    | TK_CONST type var_defs {
        auto var_defs = std::vector<ast::GlobalVarDef*>();

        for (auto manager: *$3) {
            var_defs.push_back(
                manager->to_ast_var_def<ast::ConstGlobalVarDef>($2)
            );
        }

        $$ = new ast::GlobalVarDecl(var_defs);
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
        $$ = new std::vector<std::tuple<ast::Type *, ast::Id*>*> { $1 };
    }
;

func_def_param
    : type id {
        $$ = new std::tuple<ast::Type *, ast::Id*>($1, $2);
    }
;

type
    : TK_INT {
        $$ = new ast::Int;
    }
    | TK_VOID {
        $$ = new ast::Void;
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
        $$ = new std::vector<ast::Stmt*>{};
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

initializer
    : '{' '}' {
        $$ = new ast::Aggregate({});
    }
    | '{' initializers '}' {
        $$ = new ast::Aggregate(*$2);
    }
    | '{' initializers ',' '}' {
        $$ = new ast::Aggregate(*$2);
    }
    | no_comma_expr {
        $$ = new ast::ConstInitializer($1);
    }
;

initializers
    : initializers ',' initializer {
        $1->push_back($3);
        $$ = $1;
    }
    | initializer {
        $$ = new std::vector<ast::Initializer*>{ $1 };
    }
;

decl_stmt
    : type var_defs {
        auto var_defs = std::vector<ast::VarDef*>();
        var_defs.reserve($2->size());

        for (auto manager: *$2) {
            var_defs.push_back(
                manager->to_ast_var_def<ast::VolatileVarDef>($1)
            );
        }

        $$ = new ast::VarDecl(var_defs);
    }
    | TK_CONST type var_defs {
        auto var_defs = std::vector<ast::VarDef*>();

        for (auto manager: *$3) {
            var_defs.push_back(
                manager->to_ast_var_def<ast::ConstVarDef>($2)
            );
        }

        $$ = new ast::VarDecl(var_defs);
    }
;

var_defs
    : var_defs ',' var_def {
        $1->push_back($3);
        $$ = $1;
    }
    | var_def {
        $$ = new std::vector<parser::VarDefManager*>{ $1 };
    }
;

//
// to generate dimension vector of variable declaration,
// use nullptr to represent pointer
// .e.g  a[][3][4 + x] => { nullptr, 3, (4 + x) }  b => {}
//
var_def_trace
    : var_def_trace '[' ']' {
        $1->push_back(nullptr);
        $$ = $1;
    }
    | var_def_trace '[' no_comma_expr ']' {
        $1->push_back($3);
        $$ = $1;
    }
    | {
        $$ = new std::vector<ast::Expr*>{};
    }
;

var_def
    : id var_def_trace '=' initializer {
        auto res = new parser::VarDefManager(new parser::Primitive, $1, $4);
        for (auto dim = $2->rbegin(); dim != $2->rend(); dim++) {
            if (*dim == nullptr) {
                res->wrap_pointer();
            }
            else {
                res->wrap_array(* dim);
            }
        }
        $$ = res;
    }
    | id var_def_trace {
        auto res = new parser::VarDefManager(new parser::Primitive, $1);
        for (auto dim = $2->rbegin(); dim != $2->rend(); dim++) {
            if (*dim == nullptr) {
                res->wrap_pointer();
            }
            else {
                res->wrap_array(* dim);
            }
        }
        $$ = res;
    }
;

return_stmt
    : TK_RETURN expr  {
        $$ = new ast::Return($2);
    }
    | TK_RETURN  {
        $$ = new ast::Return;
    }
;

continue_stmt
    : TK_CONTINUE  {
        $$ = new ast::Continue;
    }
;

break_stmt
    : TK_BREAK  {
        $$ = new ast::Break;
    }
;

empty_stmt
    : { $$ = new ast::Block({}); }
;

expr
    : no_comma_expr
    | no_comma_expr ',' no_comma_expr {
		$$ = new ast::Comma($1, $3);
	}
;

no_comma_expr
    : '(' no_comma_expr ')' { $$ = $2; }
    | no_comma_expr TK_LOGIC_OR no_comma_expr {
		$$ = new ast::LogicOr($1, $3);
	}
    | no_comma_expr TK_LOGIC_AND no_comma_expr {
		$$ = new ast::LogicAnd($1, $3);
	}
    | no_comma_expr TK_EQ no_comma_expr {
		$$ = new ast::Eq($1, $3);
	}
    | no_comma_expr TK_NEQ no_comma_expr {
		$$ = new ast::Neq($1, $3);
	}
    | no_comma_expr '<' no_comma_expr {
        $$ = new ast::Lt($1, $3);
	}
    | no_comma_expr '>' no_comma_expr {
		$$ = new ast::Gt($1, $3);
	}
    | no_comma_expr TK_LEQ no_comma_expr {
		$$ = new ast::Leq($1, $3);
	}
    | no_comma_expr TK_GEQ no_comma_expr {
		$$ = new ast::Geq($1, $3);
	}
    | no_comma_expr '+' no_comma_expr {
		$$ = new ast::Add($1, $3);
	}
    | no_comma_expr '-' no_comma_expr {
		$$ = new ast::Sub($1, $3);
	}
    | no_comma_expr '*' no_comma_expr {
		$$ = new ast::Mul($1, $3);
	}
    | no_comma_expr '/' no_comma_expr {
		$$ = new ast::Div($1, $3);
	}
    | no_comma_expr '%' no_comma_expr {
		$$ = new ast::Mod($1, $3);
	}
    | no_comma_expr '=' no_comma_expr {
		$$ = new ast::Assign($1, $3);
	}
    | '-' no_comma_expr %prec PREC_UNARY_OP  {
		$$ = new ast::Neg($2);
	}
    | '+' no_comma_expr %prec PREC_UNARY_OP  {
		$$ = new ast::Pos($2);
	}
    | '!' no_comma_expr %prec PREC_UNARY_OP  {
		$$ = new ast::Not($2);
	}
    | id indexing_trace {
        $$ = new ast::Indexing($1, *$2);
    }
    | func_call
    | id { $$ = $1; }
    | number                        {
		$$ = new ast::Number($1);
	}
;

indexing_trace
    : indexing_trace '[' expr ']' {
        $1->push_back($3);
        $$ = $1;
    }
    | '[' expr ']' {
        $$ = new std::vector<ast::Expr*> { $2 };
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
        $$ = new std::vector<ast::Expr*>{ $1 };
    }
;

id
    : TK_IDENT {
        $$ = new ast::Id(*$1, cur_nesting_info);
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

void yyerror(ast::CompUnit*& ast, const char* s) {
    throw std::string(s);
}
