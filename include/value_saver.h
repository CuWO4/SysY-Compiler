#ifndef VALUE_SAVER_H_
#define VALUE_SAVER_H_

#include "koopa.h"

#include <unordered_map>
#include <string>

class ValueSaver {
public:
    koopa::Id *new_id(koopa::Type *type, std::string *lit, NestingInfo *nesting_info = new NestingInfo(false), bool is_const = false, int val = 0);

    bool is_id_declared(std::string lit, NestingInfo *nesting_info);

    /* return nullptr if id is not defined */
    koopa::Id *get_id(std::string lit, NestingInfo *nesting_info);

    koopa::Const *new_const(int val);

    koopa::Undef *new_undef();

    ValueSaver();
    ~ValueSaver();

private:
    std::unordered_map<std::string, koopa::Id *> ids = {};

    std::unordered_set<koopa::Const *> consts = {};

    // singleton
    koopa::Undef *undef = nullptr;
    
    void insert_id(std::string key, koopa::Id *new_id);

    void insert_const(koopa::Const *new_const);
};

extern ValueSaver value_saver;

#endif