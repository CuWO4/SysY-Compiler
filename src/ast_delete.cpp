#include "../include/ast.h"

ast::Block::~Block() {
    if (stmt != nullptr) { delete stmt; stmt = nullptr;  }
}

ast::FuncDef::~FuncDef() {
    if (func_type != nullptr) { delete func_type; func_type = nullptr;  }
    if (id != nullptr) { delete id; id = nullptr;  }
    if (block != nullptr) { delete block; block = nullptr;  }
}

ast::CompUnit::~CompUnit() {
    if (func_def != nullptr) { delete func_def; func_def = nullptr;  }
}