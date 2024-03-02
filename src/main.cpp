#include "../include/ast.h"
#include "../include/koopa.h"

#define extern_
#include "../include/def.h"
#undef extern_

#include "../build/sysy.tab.hpp"


#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#include <string.h>

extern FILE *yyin;
extern int yyparse(ast::CompUnit *&ast);

void test() {
	ast::CompUnit *ast;
	yyparse(ast);
	std::cout << ast->debug() << std::endl;
}

// [mode, input, output]
void handle_args(int argc, const char *argv[], std::string &mode, std::string &input, std::string &output) {
	#ifndef DEBUG__

	debug_mode_koopa = false;
	debug_mode_riscv = false;

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-koopa") || !strcmp(argv[i], "-riscv")) mode = argv[i];
		else if (!strcmp(argv[i], "-o")) {
			assert(i + 1 < argc);
			output = argv[++i];
		}
		else if (!strcmp(argv[i], "-dbg-k")) debug_mode_koopa = true;
		else if (!strcmp(argv[i], "-dbg-r")) debug_mode_riscv = true;
		else input = argv[i];
	}

	#else 

	mode = "-riscv";
	input = "../test/hello/hello.c";
	output = "../test/hello/hello.koopa";
	debug_mode_koopa = false;
	debug_mode_riscv = true;

	#endif
}

int main(int argc, const char *argv[]) {
	try {
		std::string mode = {}, input = {}, output = {};
		handle_args(argc, argv, mode, input, output);

		yyin = fopen(input.c_str(), "r");
		if (yyin == nullptr) {
			throw std::string("unable to open file `") + output + '`';
		}

		std::ofstream os;
		os.open(output, std::ios::out);

		if (mode == "-test") {
			test();
			return 0;
		}

		ast::CompUnit *ast;
		yyparse(ast);

		koopa::ValueSaver value_saver;
		auto koopa = ast->to_koopa(value_saver, nullptr);

		if (mode == "-koopa") {
			os << koopa->to_string();
		} 
		else if (mode == "-riscv") {

			std::string riscv_string = "";
			riscv_trans::Info riscv_info = riscv_trans::Info();

			koopa->to_riscv(riscv_string, riscv_info);

			os << riscv_string;
		}

		// delete koopa;
		// delete ast;

	} catch (std::string &s) {
		std::cerr << "error: " << s << std::endl;
	} 

	return 0;
}
