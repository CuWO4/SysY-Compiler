#include "../include/ast.h"
#include "../include/koopa.h"
#include "../include/def.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
 
#include <string.h>

extern FILE* yyin;
extern int yyparse(ast::CompUnit*& ast);

void test() {
    ast::CompUnit* ast;
    yyparse(ast);
    std::cout << ast->debug() << std::endl;
}

void handle_args(
	int argc, const char* argv[], 
	std::string& mode, std::string& input, std::string& output
) {
	#ifndef DEBUG__
    for (int i = 1; i < argc; i++) {
	    if (!strcmp(argv[i], "-koopa") 
			|| !strcmp(argv[i], "-riscv") 
			|| !strcmp(argv[i], "-test")
		) {
		    mode = argv[i];
		}
	    else if (!strcmp(argv[i], "-o")) {
		    assert(i + 1 < argc);
		    output = argv[++i];
		}
	    else if (!strcmp(argv[i], "-dbg-k")) { 
		    debug_mode_koopa_type = true;
		}
	    else if (!strcmp(argv[i], "-dbg-r")) {
		    debug_mode_riscv = true;
		}
	    else input = argv[i];
	}

	#else 

    mode = "-koopa";
    input = "../test/hello/hello.c";
    output = "../test/hello/hello.koopa";

	#endif
}

int main(int argc, const char* argv[]) {
    try {
	    std::string mode = {}, input = {}, output = {};
	    handle_args(argc, argv, mode, input, output);

	    yyin = fopen(input.c_str(), "r");
	    if (yyin == nullptr) {
		    throw std::string("unable to open file `") + input + '`';
		}

	    std::ofstream os;
	    os.open(output, std::ios::out);

	    if (mode == "-test") {
		    test();
		    return 0;
		}

	    ast::CompUnit* ast;
	    yyparse(ast);

	    auto koopa = ast->to_koopa();

	    if (mode == "-koopa") {
		    os << koopa->to_string();
		} 
	    else if (mode == "-riscv") {

		    std::string riscv_string = "";

			/* the `trans_mode` actually does not work for `CompUnit` */
		    koopa->prog_to_riscv(riscv_string); 

		    os << riscv_string;
		}
	} catch (std::string& s) {
	    std::cerr << "error: " << s << std::endl;
	} catch (char const* const& s) {
	    std::cerr << "error: " << s << std::endl;
	}

    return 0;
}
