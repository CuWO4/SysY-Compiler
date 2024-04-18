#include "../include/nesting_info.h"

NestingInfo::NestingInfo()
    : need_suffix(false),
    nesting_level(0), nesting_count(0), pa(nullptr) {}

NestingInfo::NestingInfo(
    int nesting_level, 
    int nesting_count,
    NestingInfo *pa
) : need_suffix(true),
    nesting_level(nesting_level), 
    nesting_count(nesting_count),
    pa(pa) {}