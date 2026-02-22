#pragma once
#include <cassert>
#include "../ecs/Component.h"
#include "Transform.h"

struct DeAcceleration : ecs::Component {
    DeAcceleration() : factor_(0.995f) {}
    DeAcceleration(float factor) : factor_(factor) {}

    __CMPID_DECL__(ecs::cmp::DEACCELERATION)

        void update() override {
        auto* tr = _ent->getComponent<Transform>();
        assert(tr != nullptr);
        tr->getVel() = tr->getVel() * factor_;
    }

    float factor_;
};