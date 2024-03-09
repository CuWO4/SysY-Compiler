#include "../include/loop_tag.h"

std::stack<LoopTag> loop_tag_saver = {};

LoopTag::LoopTag(koopa::Id *continue_target, koopa::Id *break_target) 
    : continue_target(continue_target), break_target(break_target) {}