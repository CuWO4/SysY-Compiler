#include "../include/value_saver.h"

static std::string *build_name(std::string *lit, NestingInfo *nesting_info) {
    if (!nesting_info->need_suffix) return lit;
    if (*lit == "main" && nesting_info->nesting_count == 0 && nesting_info->nesting_level == 0) return lit;
    
    return new std::string(*lit 
        + '_' + std::to_string(nesting_info->nesting_level) 
        + '_' + std::to_string(nesting_info->nesting_count));
}

void ValueSaver::insert_id(std::string key, koopa::Id *new_id) { 
    ids.insert(
        std::pair<std::string, koopa::Id *>(key, new_id)
    ); 
}


void ValueSaver::insert_const(koopa::Const *new_const) {
    consts.insert(new_const);
}


koopa::Id *ValueSaver::new_id(koopa::Type *type, std::string *lit, NestingInfo *nesting_info, bool is_const, int val) {
    auto res = new koopa::Id(
        type, 
        build_name(lit, nesting_info), 
        is_const, 
        val
    );
    insert_id(*build_name(lit, nesting_info), res);
    return res;
}

bool ValueSaver::is_id_declared(std::string lit, NestingInfo *nesting_info) {
    return ids.find(*build_name(&lit, nesting_info)) != ids.end();
}

/* return nullptr if id is not defined */
koopa::Id *ValueSaver::get_id(std::string lit, NestingInfo *nesting_info) {
    if (nesting_info == nullptr) return nullptr;

    auto res = ids.find(*build_name(&lit, nesting_info));

    if (res == ids.end()) {
        return get_id(lit, nesting_info->pa);
    }
    else {
        return res->second;
    }
}

koopa::Const *ValueSaver::new_const(int val) {

    auto res = new koopa::Const(val);
    insert_const(res);
    return res;

}

koopa::Undef *ValueSaver::new_undef() {
    return undef;
}

ValueSaver::ValueSaver() {
    //TODO
    // undef = new koopa::Undef;
}

ValueSaver::~ValueSaver() { 
    // TODO in koopa::Base *ast::koopa::Id::to_koopa(), refree of pointed type
    // for (auto id_pair : ids) delete id_pair.second;
    // for (auto const_val : consts) delete const_val;
    // if (undef != nullptr) delete undef;
}