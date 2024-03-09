#ifndef LOOP_TAG_H_
#define LOOP_TAG_H_

#include "koopa.h"

#include <stack>

class LoopTag {
public:
    koopa::Id *continue_target, *break_target;

    LoopTag(koopa::Id *continue_target, koopa::Id *break_target);
};

#ifndef extern_
    #define extern_ extern
    extern_ std::stack<LoopTag> loop_tag_saver;
#else
    std::stack<LoopTag> loop_tag_saver = {};
#endif




#endif