// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

#include "../utils/Singleton.h"
#include "../ecs/EntityManager.h"

// Forward declarations
class GameState;

class Game : public Singleton<Game> {
    friend Singleton<Game>;

public:
    virtual ~Game();

    // Inicializa SDL y SDLUtils (llamado desde main)
    bool init();

    // Inicializa las entidades del juego (llamado desde main tras init)
    void initGame();

    // Bucle principal
    void start();

    // Acceso al EntityManager
    inline ecs::EntityManager* getMngr() {
        return mngr_;
    }

    // Estados del juego
    enum State {
        RUNNING, PAUSED, NEWGAME, NEWROUND, GAMEOVER
    };

    void setState(State s);

private:
    Game();
    void checkCollisions();

    ecs::EntityManager* mngr_;

    // Estados
    GameState* _state;
    GameState* _running_state;
    GameState* _paused_state;
    GameState* _newgame_state;
    GameState* _newround_state;
    GameState* _gameover_state;
};

// Macro de acceso rapido (igual que sdlutils() o ih())
inline Game& game() {
    return *Game::Instance();
}