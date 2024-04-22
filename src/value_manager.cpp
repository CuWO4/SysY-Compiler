#include "../include/value_manager.h"

#include <unordered_map>

ValueManager value_manager;

static std::string build_key(
    std::string lit, 
    NestingInfo *nesting_info, 
    bool temp_style = false
) {
    if (temp_style) {
        std::string new_lit = lit;
        new_lit[0] = '%';
        return new_lit 
            + '_' + std::to_string(nesting_info->nesting_level) 
            + '_' + std::to_string(nesting_info->nesting_count);
    }
    else {
        return lit 
            + '_' + std::to_string(nesting_info->nesting_level) 
            + '_' + std::to_string(nesting_info->nesting_count);
    }
}

/*
 * save the number of the same name identifiers
 * for converting name style from `a_1_2` to `a_2`
 */
std::unordered_map<std::string, int> existed_id_counts = {};
static std::string build_name(std::string lit, NestingInfo *nesting_info) {
    if (!nesting_info->need_suffix) return lit;
    if (nesting_info->nesting_count == 0 && nesting_info->nesting_level == 0) {
        // add no suffix for global identifiers
        return lit;
    }

    auto existed_id_count_pair = existed_id_counts.find(lit);
    int existed_id_count = 0;
    if (existed_id_count_pair == existed_id_counts.end()) {
        existed_id_counts.insert(
            std::pair<std::string, int>(lit, 1)
        );
        existed_id_count = 0;
    }
    else {
        existed_id_count = existed_id_count_pair->second;
        existed_id_count_pair->second++;
    }

    return lit + '_' + std::to_string(existed_id_count);
}


void ValueManager::insert_id(std::string key, koopa::Id *new_id) {

    if (!in_func) {
        global_ids.insert(
            std::pair<std::string, koopa::Id *>(key, new_id)
        ); 
    }
    else if (formal_param_trans_state) {
        auto current_manager = formal_params.find(current_func_id_lit);
        assert(current_manager != ids.end());

        current_manager->second->insert(
            std::pair<std::string, koopa::Id *>(key, new_id)
        ); 
    }
    else {
        auto current_manager = ids.find(current_func_id_lit);
        assert(current_manager != ids.end());

        current_manager->second->insert(
            std::pair<std::string, koopa::Id *>(key, new_id)
        ); 
    }
}


void ValueManager::insert_const(koopa::Const *new_const) {
    consts.insert(new_const);
}


koopa::Id *ValueManager::new_id(
    koopa::IdType id_type, koopa::Type *type, 
    std::string lit, NestingInfo *nesting_info
) {
    auto res = new koopa::Id(
        id_type,
        type, 
        build_name(lit, nesting_info)
    );
    insert_id(build_key(lit, nesting_info, lit.at(0) == '%'), res);
    return res;
}

koopa::Id *ValueManager::new_id(
    koopa::IdType id_type, koopa::Type *type, 
    std::string lit, int val, NestingInfo *nesting_info
) {
    auto res = new koopa::Id(
        id_type,
        type, 
        build_name(lit, nesting_info), 
        val
    );
    insert_id(build_key(lit, nesting_info, lit.at(0) == '%'), res);
    return res;
}

bool is_id_declared_impl(
    std::unordered_map<std::string, koopa::Id *> &id_manager, 
    std::string lit, NestingInfo *nesting_info
) {
    return id_manager.find(build_key(lit, nesting_info)) != id_manager.end()
        || id_manager.find(build_key(lit, nesting_info, true)) != id_manager.end();
}

bool ValueManager::is_id_declared(std::string lit, NestingInfo *nesting_info) {
    if (!in_func) {
        return is_id_declared_impl(global_ids, lit, nesting_info);
    }
    
    auto current_manager = ids.find(current_func_id_lit);
    assert(current_manager != ids.end());
    auto param_manager = formal_params.find(current_func_id_lit);
    assert(param_manager != ids.end());

    return is_id_declared_impl(*current_manager->second, lit, nesting_info)
        || is_id_declared_impl(*param_manager->second, lit, nesting_info)
        || is_id_declared_impl(global_ids, lit, nesting_info);
}

/*
 * search identifier in `id_manager`, return it if exist; nullptr otherwise.
 * if there is a identifier starting with `@` and one starting with `%`,
 * which share the same name, return the later. 
 */
static koopa::Id * get_id_impl(
    std::unordered_map<std::string, koopa::Id *> &id_manager,
    std::string lit, NestingInfo *nesting_info
) {
    auto res = id_manager.find(build_key(lit, nesting_info));
    auto res_temp_style = id_manager.find(
        build_key(lit, nesting_info, true)
    );

    if (res != id_manager.end() 
        || res_temp_style != id_manager.end()) {
        auto not_empty_res_id = res_temp_style != id_manager.end()
            ? res_temp_style->second /* prefer temporary style */
            : res->second;
        return not_empty_res_id;
    }

    return nullptr;
}

koopa::Id *ValueManager::get_id(std::string lit, NestingInfo *nesting_info) {
    if (nesting_info->nesting_level == 0) {
        return get_id_impl(global_ids, lit, nesting_info);
    }
    else {
        auto current_manager = ids.find(current_func_id_lit);
        assert(current_manager != ids.end());
        auto param_manager = formal_params.find(current_func_id_lit);
        assert(param_manager != ids.end());

        auto res = get_id_impl(*current_manager->second, lit, nesting_info);
        auto param = get_id_impl(*param_manager->second, lit, nesting_info);
        return res != nullptr || param != nullptr
            ? (res != nullptr ? res : param)
            : get_id(lit, nesting_info->pa);
    }
}

koopa::Id *ValueManager::get_func_id(std::string lit, NestingInfo *nesting_info) {
    if (nesting_info->nesting_level == 0) {
        auto res = get_id_impl(global_ids, lit, nesting_info);
        return res->type->get_type_id() != koopa::type::FuncType
            ? res
            : nullptr;
    }
    else {
        auto current_manager = ids.find(current_func_id_lit);
        assert(current_manager != ids.end());

        auto res = get_id_impl(*current_manager->second, lit, nesting_info);
        return res != nullptr && res->type->get_type_id() != koopa::type::FuncType
            ? res
            : get_id(lit, nesting_info->pa);
    }
}

void ValueManager::enter_func(std::string func_id_lit) {
    in_func = true;
    current_func_id_lit = (func_id_lit.at(0) == '@' ? "" : "@") + func_id_lit;
    /* 
     * when inserting a pair with the same key as an existing pair, the insert 
     * will fail (nothing will happen). so don’t worry that re-entering the function 
     * will cause the original saved variables to be lost.
     */
    ids.insert(
        std::pair<std::string, std::unordered_map<std::string, koopa::Id *> *>(
            current_func_id_lit,
            new std::unordered_map<std::string, koopa::Id *>({})
        )
    );
    formal_params.insert(
        std::pair<std::string, std::unordered_map<std::string, koopa::Id *> *>(
            current_func_id_lit,
            new std::unordered_map<std::string, koopa::Id *>({})
        )
    );
}

void ValueManager::leave_func() {
    in_func = false;
}

void ValueManager::enter_formal_params() {
    formal_param_trans_state = true;
}

void ValueManager::leave_formal_params() {
    formal_param_trans_state = false;
}

std::vector<koopa::Id *> ValueManager::get_func_ids(std::string func_id_lit) {
    auto func_manager = ids.find(func_id_lit);

    if (func_manager == ids.end()) {
        throw "function `" + func_id_lit + "` not exists when calling `get_func_ids`";
    }

    std::vector<koopa::Id *> res;

    res.reserve(func_manager->second->size());
    for (auto id_pair: *func_manager->second) {
        res.push_back(id_pair.second);
    }

    return res; 
}

std::vector<koopa::Id *> ValueManager::get_global_ids() {
    std::vector<koopa::Id *> res;

    res.reserve(global_ids.size());
    for (auto id_pair: global_ids) {
        res.push_back(id_pair.second);
    }

    return res; 
}

koopa::Const *ValueManager::new_const(int val) {

    auto res = new koopa::Const(val);
    insert_const(res);
    return res;

}

koopa::Undef *ValueManager::new_undef() {
    return undef;
}

ValueManager::ValueManager()
    : in_func(false), 
    formal_param_trans_state(false),
    current_func_id_lit(""),
    global_ids({}),
    ids({}),
    formal_params({}),
    consts({}),
    undef(nullptr)
{
    //TODO
    // undef = new koopa::Undef;
}

ValueManager::~ValueManager() { 
}