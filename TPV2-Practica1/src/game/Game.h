// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

#include "../utils/Singleton.h"
#include "../ecs/EntityManager.h"

// Forward declarations
class GameState;
class FighterUtils;
class AsteroidsUtils;

class Game : public Singleton<Game> {
    friend Singleton<Game>;

public:
    virtual ~Game();

    bool init();
    void initGame();
    void start();

    inline ecs::EntityManager* getMngr() { return mngr_; }

    enum State { RUNNING, PAUSED, NEWGAME, NEWROUND, GAMEOVER };
    void setState(State s);

    // Public para que los estados puedan llamarlo
    void checkCollisions();

private:
    Game();

    ecs::EntityManager* mngr_;

    GameState* _state;
    GameState* _running_state;
    GameState* _paused_state;
    GameState* _newgame_state;
    GameState* _newround_state;
    GameState* _gameover_state;

    FighterUtils* _fu;
    AsteroidsUtils* _au;
};

inline Game& game() {
    return *Game::Instance();
}