// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

// Clase base abstracta para todos los estados del juego.
// Sigue el patron State descrito en el enunciado.

class GameState {
public:
    GameState() {}
    virtual ~GameState() {}

    virtual void enter() = 0;
    virtual void leave() = 0;
    virtual void update() = 0;
};