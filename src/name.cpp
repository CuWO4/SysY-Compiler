#include "../include/name.h"
#include "../include/def.h"

std::string *new_block_name() {
    return new std::string("%LLB_" + std::to_string(block_count++));
}

std::string *new_id_name() {
    return new std::string('%' + std::to_string(tmp_var_count++));
}