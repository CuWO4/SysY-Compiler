#include "../include/ast.h"
#include "../include/name.h"

namespace ast {

bool Expr::is_assignable() const {
    return false;
}

koopa_trans::Blocks* Expr::assign(const Expr* rv) const {
    assert(is_assignable());
    return nullptr;
}

bool Id::is_assignable() const {
    return true;
}

koopa_trans::Blocks* Id::assign(const Expr* rv) const {
    auto res = new koopa_trans::Blocks;
    auto rv_stmts = rv->to_koopa();

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

bool Indexing::is_assignable() const {
    return true;
}

koopa_trans::Blocks* Indexing::assign(const Expr* rv) const {
    auto res = get_pointer();
    auto pointer = static_cast<koopa::Id*>(res->get_last_val());

    auto rv_stmts = rv->to_koopa();
    *res += *rv_stmts;
    
    *res += new koopa::StoreValue(
        rv_stmts->get_last_val(),
        pointer
    );

    res->set_last_val(rv_stmts->get_last_val());
    
    return res;
}

}