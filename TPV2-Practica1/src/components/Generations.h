#pragma once
#include "../ecs/Component.h"

struct Generations : ecs::Component {
    Generations() : numGenerations_(0) {}
    Generations(int num) : numGenerations_(num) {}

    __CMPID_DECL__(ecs::cmp::GENERATIONS)

        int numGenerations_;
};