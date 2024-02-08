#include "../include/ast.h"
#include "../include/koopa.h"

#include "../build/sysy.tab.hpp"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

extern FILE *yyin;
extern int yyparse(ast::CompUnit *&ast);

int main(int argc, const char *argv[]) {
	assert(argc == 5);

	auto mode = argv[1];
	auto input = argv[2];
	auto output = argv[4];

	yyin = fopen(input, "r");
	assert(yyin);

	ast::CompUnit *ast;
	auto ret = yyparse(ast);
	assert(!ret);

	delete ast;
    
	return 0;
}
