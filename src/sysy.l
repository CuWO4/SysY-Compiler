%option noyywrap
%option nounput
%option noinput

%{

#include <cstdlib>
#include <string>

#include "sysy.tab.hpp"

%}

WhiteSpace      [ \t\n\r]
LineComment     "//".*
BlockComment    "/"\*.*?\*"/"

Identifier      [a-zA-Z_][a-zA-Z0-9_]*

Decimal         [1-9][0-9]*
Octal           0[0-7]*
Hexadecimal     0[xX][0-9a-fA-F]+

%%

{WhiteSpace}
{LineComment}
{BlockComment}

"||"            { return TK_LOGIC_OR; }
"&&"            { return TK_LOGIC_AND; }
"=="            { return TK_EQ; }
"!="            { return TK_NEQ; }
"<="            { return TK_LEQ; }
">="            { return TK_GEQ; }

"int"           { return TK_INT; }
"void"          { return TK_VOID; }
"return"        { return TK_RETURN; }
"const"         { return TK_CONST; }
"if"            { return TK_IF; }
"else"          { return TK_ELSE; }
"while"         { return TK_WHILE; }
"for"           { return TK_FOR; }
"continue"      { return TK_CONTINUE; }
"break"         { return TK_BREAK; }

{Identifier}    { yylval.str_val = new std::string(yytext); return TK_IDENT; }

{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); return TK_INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); return TK_INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); return TK_INT_CONST; }

.               { return yytext[0]; }

%%
