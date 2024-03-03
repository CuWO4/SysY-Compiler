#include "../include/block_name.h"
#include "../include/def.h"

std::string *new_block_name() {
    return new std::string("%block_" + std::to_string(block_count++));
}