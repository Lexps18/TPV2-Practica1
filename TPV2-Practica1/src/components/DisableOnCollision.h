#pragma once
#include "../ecs/Component.h"

// Componente que marca la entidad como muerta cuando colisiona.
// La logica de colision en Game::checkCollisions() llama a
// ent_->setAlive(false) directamente, pero este componente
// sirve como "tag" para identificar entidades que deben
// desactivarse al ser golpeadas.

struct DisableOnCollision : ecs::Component {

    __CMPID_DECL__(ecs::cmp::DISABLEONCOLLISION)

        DisableOnCollision() {}

    // No hace nada por si solo; es Game::checkCollisions() quien
    // detecta la colision y llama a setAlive(false) sobre la entidad.
    void update() override {}
};