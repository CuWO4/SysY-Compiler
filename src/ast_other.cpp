#include "ast.h"

namespace ast {

bool Expr::has_side_effect() const {
    return false;
}

bool BinaryExpr::has_side_effect() const {
    return lv->has_side_effect() || rv->has_side_effect();
}

bool Assign::has_side_effect() const {
    return true;
}

bool UnaryExpr::has_side_effect() const {
    return lv->has_side_effect();
}

bool Indexing::has_side_effect() const {
    for (auto index: indexes) {
        if (index->has_side_effect()) {
            return true;
        }
    }
    return false;
}

bool FuncCall::has_side_effect() const {
    // TODO  optimize it when no side effect behavior in function body occurs
    return true;
}

unsigned ConstInitializer::get_dim() const { return 0; }

unsigned Aggregate::get_dim() const {
    unsigned max_dim = 0;
    for (auto initializer: initializers) {
        max_dim = std::max(max_dim, initializer->get_dim());
    }
    return max_dim + 1;
}

}