#pragma once
#include "../ecs/Component.h"
#include "../ecs/Entity.h"
#include "Transform.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"

struct TowardDestination : ecs::Component {
    __CMPID_DECL__(ecs::cmp::TOWARDDESTINATION)

        TowardDestination() : _speed(0.5f) {}
    TowardDestination(float speed) : _speed(speed) {}

    void initComponent() override {
        pickNewDestination();
    }

    void update() override {
        auto* tr = _ent->getComponent<Transform>();
        if (tr == nullptr) return;

        Vector2D p = tr->getPos();
        Vector2D dir = _dest - p;

        // Si ha llegado al destino, elegir otro
        if (dir.magnitude() < 5.0f)
            pickNewDestination();

        // Actualizar velocidad hacia el destino
        tr->getVel() = dir.normalize() * _speed;
    }

private:
    void pickNewDestination() {
        auto& rng = sdlutils().rand();
        _dest = Vector2D(
            (float)rng.nextInt(50, sdlutils().width() - 50),
            (float)rng.nextInt(50, sdlutils().height() - 50)
        );
    }

    Vector2D _dest;
    float    _speed;
};