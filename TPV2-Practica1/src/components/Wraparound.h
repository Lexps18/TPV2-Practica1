#pragma once
#include <cassert>
#include "../ecs/Component.h"
#include "../ecs/Entity.h"
#include "Transform.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"

// Cuando la entidad sale de la pantalla aparece por el lado contrario.
// Se usa tanto para el caza como para los asteroides.

struct WrapAround : ecs::Component {

    __CMPID_DECL__(ecs::cmp::WRAPAROUND)

        WrapAround() {}

    void update() override {
        auto* tr = _ent->getComponent<Transform>();
        assert(tr != nullptr);

        float screenW = (float)sdlutils().width();
        float screenH = (float)sdlutils().height();
        float w = tr->getWidth();
        float h = tr->getHeight();

        Vector2D& pos = tr->getPos();

        // Sale por la derecha -> aparece por la izquierda
        if (pos.getX() > screenW)
            pos = Vector2D(-w, pos.getY());
        // Sale por la izquierda -> aparece por la derecha
        else if (pos.getX() + w < 0.0f)
            pos = Vector2D(screenW, pos.getY());

        // Sale por abajo -> aparece por arriba
        if (pos.getY() > screenH)
            pos = Vector2D(pos.getX(), -h);
        // Sale por arriba -> aparece por abajo
        else if (pos.getY() + h < 0.0f)
            pos = Vector2D(pos.getX(), screenH);
    }
};