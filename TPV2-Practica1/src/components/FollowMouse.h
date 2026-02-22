#pragma once
#include <cmath>
#include <cassert>
#include "../ecs/Component.h"
#include "Transform.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct FollowMouse : ecs::Component {
    FollowMouse() {}

    __CMPID_DECL__(ecs::cmp::FOLLOWMOUSE)

        void update() override {
        auto* tr = _ent->getComponent<Transform>();
        assert(tr != nullptr);

        auto& ihdlr = ih();
        Vector2D mousePos(
            static_cast<float>(ihdlr.getMousePos().first),
            static_cast<float>(ihdlr.getMousePos().second)
        );

        // Centro del caza
        Vector2D center = tr->getPos() + Vector2D(tr->getWidth() / 2.0f, tr->getHeight() / 2.0f);

        // Vector del caza al raton
        Vector2D dir = mousePos - center;

        // Angulo en grados (+90 para alinear el sprite que apunta hacia arriba)
        float angle = atan2f(dir.getY(), dir.getX());
        tr->setRot(angle * 180.0f / (float)M_PI + 90.0f);
    }
};