#ifndef LOOP_TAG_H_
#define LOOP_TAG_H_

#include "koopa.h"

#include <stack>

class LoopTag {
public:
    LoopTag(koopa::Label continue_target, koopa::Label break_target);

    koopa::Label get_continue_target() const;
    koopa::Label get_break_target() const;

private:
    koopa::Label continue_target;
    koopa::Label break_target;
};

extern std::stack<LoopTag> loop_tag_manager;

#endif