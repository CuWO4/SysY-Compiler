#include "../include/ast.h"

namespace ast {

bool BinaryExpr::has_side_effect() const {
    return op == op::ASSIGN || lv->has_side_effect() || rv->has_side_effect();
}

bool UnaryExpr::has_side_effect() const {
    return lv->has_side_effect();
}

bool Id::has_side_effect() const {
    return false;
}

bool Number::has_side_effect() const {
    return false;
}

}