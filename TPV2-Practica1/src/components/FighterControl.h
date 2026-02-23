#pragma once
#include <cassert>
#include "../ecs/Component.h"
#include "../ecs/Entity.h"
#include "Transform.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"

// Controla el caza:
// - Flechas izquierda/derecha: girar 5 grados
// - W o flecha arriba: acelerar (con sonido thrust)

struct FighterControl : ecs::Component {
    __CMPID_DECL__(ecs::cmp::FIGHTERCONTROL)

        FighterControl(float thrust = 0.2f, float speedLimit = 3.0f)
        : _thrust(thrust), _speedLimit(speedLimit) {
    }

    void update() override {
        auto* tr = _ent->getComponent<Transform>();
        assert(tr != nullptr);

        auto& ihdlr = ih();

        // Girar con flechas izquierda/derecha
        if (ihdlr.isKeyDown(SDL_SCANCODE_LEFT))
            tr->setRot(tr->getRot() - 5.0f);
        if (ihdlr.isKeyDown(SDL_SCANCODE_RIGHT))
            tr->setRot(tr->getRot() + 5.0f);

        // Acelerar con W o flecha arriba
        if (ihdlr.isKeyDown(SDL_SCANCODE_W) || ihdlr.isKeyDown(SDL_SCANCODE_UP)) {
            Vector2D vel = tr->getVel();
            float rot = tr->getRot();

            Vector2D newVel = vel + Vector2D(0.0f, -1.0f).rotate(rot) * _thrust;
            if (newVel.magnitude() > _speedLimit)
                newVel = newVel.normalize() * _speedLimit;

            tr->getVel() = newVel;

            // Sonido de empuje
            sdlutils().soundEffects().at("thrust").play();
        }
    }

private:
    float _thrust;
    float _speedLimit;
};