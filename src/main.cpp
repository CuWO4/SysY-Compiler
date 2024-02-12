#include "../include/ast.h"
#include "../include/koopa.h"

#include "../build/sysy.tab.hpp"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include <string.h>

extern FILE *yyin;
extern int yyparse(ast::CompUnit *&ast);

// #define DEBUG__

int main(int argc, const char *argv[]) {
	#ifndef DEBUG__
	assert(argc == 5);

	auto mode = argv[1];
	auto input = argv[2];
	auto output = argv[4];
	#else 
	auto mode = "-test";
	auto input = "../test/hello/hello.c";
	auto output = "../test/hello/hello.koopa";
	#endif

	yyin = fopen(input, "r");
	assert(yyin);

	std::ofstream os;
	os.open(output, std::ios::out);

	ast::CompUnit *ast;
	auto ret = yyparse(ast);
	assert(!ret);

	auto koopa = ast->to_koopa();

	if (!strcmp(mode, "-test")) {
		std::cout << koopa->to_string() << std::endl;
	}
	else if (!strcmp(mode, "-koopa")) {
		os << koopa->to_string();
	} 
	else if (!strcmp(mode, "-riscv")) {
		os << koopa->to_riscv();
	}
	
	// delete koopa;		//! re-free bug in ast_to_koopa.cpp: ast::BinaryExpr::to_koopa() and ast::UnaryExpr::to_koopa()
	// delete ast;			//? fix it by move the ownership of values to koopa::Program

	return 0;
}
