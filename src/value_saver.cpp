#include "../include/value_saver.h"

#include <unordered_map>

ValueSaver value_saver;

static std::string *build_key(std::string *lit, NestingInfo *nesting_info, bool temp_style = false) {
    if (temp_style) {
        std::string new_lit = *lit;
        new_lit[0] = '%';
        return new std::string(
            new_lit 
            + '_' + std::to_string(nesting_info->nesting_level) 
            + '_' + std::to_string(nesting_info->nesting_count)
        );
    }
    else {
        return new std::string(
            *lit 
            + '_' + std::to_string(nesting_info->nesting_level) 
            + '_' + std::to_string(nesting_info->nesting_count)
        );
    }
}

std::unordered_map<std::string, int> existed_id_counts = {};
static std::string *build_name(std::string *lit, NestingInfo *nesting_info) {
    if (!nesting_info->need_suffix) return lit;
    if (*lit == "@main" && nesting_info->nesting_count == 0 && nesting_info->nesting_level == 0) return lit;

    auto existed_id_count_pair = existed_id_counts.find(*lit);
    int existed_id_count = 0;
    if (existed_id_count_pair == existed_id_counts.end()) {
        existed_id_counts.insert(
            std::pair<std::string, int>(*lit, 1)
        );
        existed_id_count = 0;
    }
    else {
        existed_id_count = existed_id_count_pair->second;
        existed_id_count_pair->second++;
    }

    return new std::string(*lit + '_' + std::to_string(existed_id_count));
}

void ValueSaver::insert_id(std::string key, koopa::Id *new_id) { 
    ids.insert(
        std::pair<std::string, koopa::Id *>(key, new_id)
    ); 
}


void ValueSaver::insert_const(koopa::Const *new_const) {
    consts.insert(new_const);
}


koopa::Id *ValueSaver::new_id(koopa::Type *type, std::string *lit, NestingInfo *nesting_info, 
                              bool is_const, int val, bool is_formal_param) {
    auto res = new koopa::Id(
        type, 
        build_name(lit, nesting_info), 
        is_const, 
        val,
        is_formal_param
    );
    insert_id(*build_key(lit, nesting_info), res);
    return res;
}

bool ValueSaver::is_id_declared(std::string lit, NestingInfo *nesting_info) {
    return ids.find(*build_key(&lit, nesting_info)) != ids.end()
        || ids.find(*build_key(&lit, nesting_info, true)) != ids.end();
}

/* return nullptr if id is not defined */
koopa::Id *ValueSaver::get_id(std::string lit, NestingInfo *nesting_info) {
    if (nesting_info == nullptr) return nullptr;

    auto res = ids.find(*build_key(&lit, nesting_info));
    auto res_temp_style = ids.find(*build_key(&lit, nesting_info, true));

    if (res == ids.end() && res_temp_style == ids.end()) {
        return get_id(lit, nesting_info->pa);
    }
    else {
        if (res_temp_style != ids.end()) return res_temp_style->second;
        else return res->second;
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