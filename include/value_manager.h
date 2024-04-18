#ifndef VALUE_MANAGER_H_
#define VALUE_MANAGER_H_

#include "koopa.h"

#include <unordered_map>
#include <string>

/**
 * manage the values of koopa. it also manage the consts in order to delete them and avoid 
 * memory re-free bug
 *
 * manage values according to function classification (or global). supports finding all local 
 * variables defined within a function. call `enter_func` while entering a function definition; 
 * call `leave_func` while leaving.
 */
class ValueManager {
// simulate, but not change the interface. add method `ids_of_func`, and return a iterable type
public:
    koopa::Id *new_id(
        koopa::IdType id_type, koopa::Type *type, std::string lit, 
        NestingInfo *nesting_info = new NestingInfo(), 
        bool is_const = false, int val = 0
    );

    bool is_id_declared(std::string lit, NestingInfo *nesting_info);

    /* return identifier if exist one; otherwise nullptr */
    koopa::Id *get_id(std::string lit, NestingInfo *nesting_info);
    /* find identifier with type `koopa::type::FuncType` */
    koopa::Id *get_func_id(std::string lit, NestingInfo *nesting_info);

    /*
     * enter/leave function, change the inner state
     */
    void enter_func(std::string func_id_lit);
    void leave_func();
    /*
     * when in formal parameters translation mode, `new_id` will not save
     * id to `ids`; in other word, they will not be get when calling 
     * `get_func_ids`
     */
    void enter_formal_params();
    void leave_formal_params();

    /**
     * @return  vector of ids defined in certain function
     * @throw   <std::string> if function does not exist
     */
    std::vector<koopa::Id *> get_func_ids(std::string func_id_lit);
    std::vector<koopa::Id *> get_global_ids();

    koopa::Const *new_const(int val);

    koopa::Undef *new_undef();

    ValueManager();
    ~ValueManager();

private:
    bool in_func;
    bool formal_param_trans_state;
    std::string current_func_id_lit;

    std::unordered_map<std::string, koopa::Id *> global_ids;
    std::unordered_map<std::string, std::unordered_map<std::string, koopa::Id *> *> ids;
    std::unordered_map<std::string, std::unordered_map<std::string, koopa::Id *> *> formal_params;

    std::unordered_set<koopa::Const *> consts;

    // singleton
    koopa::Undef *undef;
    
    void insert_id(std::string key, koopa::Id *new_id);

    void insert_const(koopa::Const *new_const);
};

extern ValueManager value_manager;

#endif