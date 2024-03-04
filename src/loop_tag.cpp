#include "../include/loop_tag.h"

LoopTag::LoopTag(koopa::Id *continue_target, koopa::Id *break_target) 
    : continue_target(continue_target), break_target(break_target) {}