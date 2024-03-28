#ifndef NESTING_INFO_H_
#define NESTING_INFO_H_

class NestingInfo {
public:
    bool need_suffix = true;
    int nesting_level = 0, nesting_count = 0;
    NestingInfo *pa = nullptr;

    NestingInfo(int nl = 0, int nc = 0, NestingInfo *pa = nullptr) :
        nesting_level(nl), nesting_count(nc), pa(pa) {}

    NestingInfo(bool need_suffix): need_suffix(need_suffix) {}
};

#endif