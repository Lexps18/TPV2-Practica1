#pragma once
#include "../ecs/Component.h"
#include "../ecs/Entity.h"
#include "../ecs/EntityManager.h"
#include "Transform.h"
#include "ecs_defs.h"

// El asteroide actualiza su velocidad para seguir al caza.
// Formula del enunciado: v = v.rotate(v.angle(q-p) > 0 ? 1.0f : -1.0f)

struct Follow : ecs::Component {
    __CMPID_DECL__(ecs::cmp::FOLLOW)

        Follow() {}

    void update() override {
        auto* tr = _ent->getComponent<Transform>();
        if (tr == nullptr) return;

        // Obtener posicion del caza via handler
        auto* fighter = _ent->getMngr()->getHandler(ecs::hdlr::FIGHTER_HDLR);
        if (fighter == nullptr || !fighter->isAlive()) return;

        auto* fighterTr = fighter->getComponent<Transform>();
        if (fighterTr == nullptr) return;

        Vector2D p = tr->getPos();
        Vector2D q = fighterTr->getPos();
        Vector2D v = tr->getVel();

        // Girar ligeramente hacia el caza
        float ang = v.angle(q - p);
        tr->getVel() = v.rotate(ang > 0 ? 1.0f : -1.0f);
    }
};