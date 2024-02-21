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

"||"            { printf("TK_LOGIC_OR found\n"); return TK_LOGIC_OR;  }
"&&"            { printf("TK_LOGIC_AND found\n"); return TK_LOGIC_AND;  }
"=="            { printf("TK_EQ found\n"); return TK_EQ;  }
"!="            { printf("TK_NEQ found\n"); return TK_NEQ;  }
"<="            { printf("TK_LEQ found\n"); return TK_LEQ;  }
">="            { printf("TK_GEQ found\n"); return TK_GEQ;  }

"int"           { printf("TK_INT found\n"); return TK_INT;  }
"return"        { printf("TK_RETURN found\n"); return TK_RETURN;  }
"const"         { printf("TK_CONST found\n"); return TK_CONST;  }

{Identifier}    { yylval.str_val = new std::string(yytext); printf("TK_IDENT found\n"); return TK_IDENT;  }

{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); printf("TK_INT_CONST found\n"); return TK_INT_CONST;  }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); printf("TK_INT_CONST found\n"); return TK_INT_CONST;  }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); printf("TK_INT_CONST found\n"); return TK_INT_CONST;  }

.               { printf("%c found\n", *yytext); return yytext[0];  }

%%
