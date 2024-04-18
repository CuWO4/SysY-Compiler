#ifndef NESTING_INFO_H_
#define NESTING_INFO_H_

class NestingInfo {
public:
    bool            need_suffix;
    int             nesting_level;
    int             nesting_count;
    NestingInfo    *pa;

    // need suffix = false
    NestingInfo();

    NestingInfo(
        int nesting_level, 
        int nesting_count, 
        NestingInfo *pa
    );

};

#endif