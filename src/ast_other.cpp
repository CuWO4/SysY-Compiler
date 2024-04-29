#include "../include/ast.h"

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

bool Expr::is_assignable() const {
    return false;
}

koopa_trans::Blocks* Expr::assign(const Expr* rval) const {
    assert(is_assignable());
    return nullptr;
}

bool Id::is_assignable() const {
    return true;
}

koopa_trans::Blocks* Id::assign(const Expr* rval) const {
    auto res = new koopa_trans::Blocks;
    auto rv_stmts = rval->to_koopa();

    *res += *rv_stmts;

    auto id_koopa = value_manager.get_id('@' + lit, nesting_info);

    if (id_koopa == nullptr) {
        throw "undeclared identifier `" + lit + '`';
    }
    if (id_koopa->is_const()) {
        throw "assigning to a const identifier `" + lit + '`';
    }

    *res += new koopa::StoreValue(
        res->get_last_val(),
        value_manager.get_id('@' + lit, nesting_info)
    );

    res->set_last_val(id_koopa);

    return res;
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