#pragma once
#include "../ecs/Component.h"
#include "../ecs/Entity.h"
#include "Transform.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"

// Cuando la entidad sale completamente de la pantalla,
// aparece en una posicion aleatoria en otro borde.

struct TeleportOnExit : ecs::Component {
    __CMPID_DECL__(ecs::cmp::TELEPORTONEXIT)

        void update() override {
        auto* tr = _ent->getComponent<Transform>();
        if (tr == nullptr) return;

        float x = tr->getPos().getX();
        float y = tr->getPos().getY();
        float w = tr->getWidth();
        float h = tr->getHeight();
        float sw = (float)sdlutils().width();
        float sh = (float)sdlutils().height();

        bool outLeft = x + w < 0;
        bool outRight = x > sw;
        bool outTop = y + h < 0;
        bool outBottom = y > sh;

        if (!outLeft && !outRight && !outTop && !outBottom) return;

        auto& rng = sdlutils().rand();
        int border = rng.nextInt(0, 4);
        float nx, ny;
        switch (border) {
        case 0: nx = (float)rng.nextInt(0, (int)(sw - w)); ny = -h;      break;
        case 1: nx = (float)rng.nextInt(0, (int)(sw - w)); ny = sh;      break;
        case 2: nx = -w;  ny = (float)rng.nextInt(0, (int)(sh - h));     break;
        default:nx = sw;  ny = (float)rng.nextInt(0, (int)(sh - h));     break;
        }

        tr->getPos() = Vector2D(nx, ny);
    }
};