#ifndef LOOP_TAG_H_
#define LOOP_TAG_H_

#include "koopa.h"

#include <stack>

class LoopTag {
public:
    koopa::Label continue_target, break_target;

    LoopTag(koopa::Label continue_target, koopa::Label break_target);
};

extern std::stack<LoopTag> loop_tag_manager;

#endif