#pragma once
#include <cassert>
#include "../ecs/Component.h"
#include "../ecs/Entity.h"
#include "Transform.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"

// Controla la aceleracion del caza con W (o flecha arriba).
// La formula de aceleracion y limite de velocidad sigue el enunciado.

struct FighterControl : ecs::Component {

    __CMPID_DECL__(ecs::cmp::FIGHTERCONTROL)

        FighterControl(float thrust = 0.2f, float speedLimit = 3.0f)
        : _thrust(thrust), _speedLimit(speedLimit) {
    }

    void update() override {
        auto* tr = _ent->getComponent<Transform>();
        assert(tr != nullptr);

        // Tecla W o flecha arriba para acelerar
        auto& ihdlr = ih();
        if (ihdlr.isKeyDown(SDL_SCANCODE_W) || ihdlr.isKeyDown(SDL_SCANCODE_UP)) {
            Vector2D vel = tr->getVel();
            float    rot = tr->getRot();

            // Formula del enunciado:
            // new_vel = vel + Vector2D(0,-1).rotate(r) * thrust
            Vector2D newVel = vel + Vector2D(0.0f, -1.0f).rotate(rot) * _thrust;

            // Limitar la magnitud a speedLimit
            if (newVel.magnitude() > _speedLimit)
                newVel = newVel.normalize() * _speedLimit;

            tr->getVel() = newVel;
        }
    }

private:
    float _thrust;
    float _speedLimit;
};