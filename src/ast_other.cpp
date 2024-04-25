#include "../include/ast.h"

namespace ast {

bool BinaryExpr::has_side_effect() const {
    return lv->has_side_effect() || rv->has_side_effect();
}

bool Assign::has_side_effect() const {
    return true;
}

bool UnaryExpr::has_side_effect() const {
    return lv->has_side_effect();
}

bool Id::has_side_effect() const {
    return false;
}

bool FuncCall::has_side_effect() const {
    // TODO  can be handled better
    return true;
}

bool Number::has_side_effect() const {
    return false;
}

int Int::get_dim() const { return 0; }

int Void::get_dim() const { return 0; }

int Pointer::get_dim() const { return pointed_type->get_dim() + 1; }

int Array::get_dim() const { return element_type->get_dim() + 1; }

int ConstInitializer::get_dim() const { return 0; }

int Aggregate::get_dim() const {
    int max_dim = 0;
    for (auto initializer: initializers) {
        max_dim = std::max(max_dim, initializer->get_dim());
    }
    return max_dim + 1;
}

}