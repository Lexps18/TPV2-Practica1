#pragma once
#include "../ecs/Component.h"

struct Health : ecs::Component {
    Health() : lives_(3) {}
    Health(int lives) : lives_(lives) {}

    __CMPID_DECL__(ecs::cmp::HEALTH)

        int lives_;
};