#include "../include/loop_tag.h"

std::stack<LoopTag> loop_tag_manager = {};

LoopTag::LoopTag(koopa::Label continue_target, koopa::Label break_target) 
    : continue_target(continue_target), break_target(break_target) {}