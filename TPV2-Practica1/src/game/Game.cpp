// This file is part of the course TPV2@UCM - Samir Genaim

#include "Game.h"
#include "GameStates.h"
#include "FighterUtils.h"
#include "AsteroidsUtils.h"

#include <iostream>
#include "../components/Transform.h"
#include "../components/Gun.h"
#include "../components/Health.h"
#include "../components/Generations.h"
#include "../components/DisableOnCollision.h"
#include "../ecs/EntityManager.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"
#include "../utils/Collisions.h"
#include "ecs_defs.h"

Game::Game() :
    mngr_(nullptr),
    _state(nullptr),
    _running_state(nullptr),
    _paused_state(nullptr),
    _newgame_state(nullptr),
    _newround_state(nullptr),
    _gameover_state(nullptr),
    _fu(nullptr),
    _au(nullptr),
    _stateChanged(false)
{
}

Game::~Game() {
    delete _running_state;
    delete _paused_state;
    delete _newgame_state;
    delete _newround_state;
    delete _gameover_state;
    delete _fu;
    delete _au;
    delete mngr_;
    if (InputHandler::HasInstance()) InputHandler::Release();
    if (SDLUtils::HasInstance())     SDLUtils::Release();
}

bool Game::init() {
    if (!SDLUtils::Init("Asteroids", 800, 600,
        "resources/config/asteroid.resources.json")) {
        std::cerr << "Error inicializando SDLUtils" << std::endl;
        return false;
    }
    if (!InputHandler::Init()) {
        std::cerr << "Error inicializando InputHandler" << std::endl;
        return false;
    }
    sdlutils().showCursor();
    return true;
}

void Game::initGame() {
    mngr_ = new ecs::EntityManager();
    _fu = new FighterUtils(mngr_);
    _au = new AsteroidsUtils(mngr_);

    _fu->create_fighter();

    _running_state = new RunningState(this, _fu, _au);
    _paused_state = new PausedState(this, _fu, _au);
    _newgame_state = new NewGameState(this, _fu, _au);
    _newround_state = new NewRoundState(this, _fu, _au);
    _gameover_state = new GameOverState(this, _fu, _au);

    _state = _newgame_state;
    _state->enter();
}

void Game::setState(State s) {
    _state->leave();
    switch (s) {
    case RUNNING:  _state = _running_state;  break;
    case PAUSED:   _state = _paused_state;   break;
    case NEWGAME:  _state = _newgame_state;  break;
    case NEWROUND: _state = _newround_state; break;
    case GAMEOVER: _state = _gameover_state; break;
    }
    _stateChanged = true;
    _state->enter();
}

void Game::start() {
    bool exit = false;
    auto& ihdlr = ih();
    auto& vt = sdlutils().virtualTimer();
    vt.resetTime();

    while (!exit) {
        // Registrar el tiempo real actual en el timer virtual
        Uint32 startTime = (Uint32)vt.regCurrTime();
        ihdlr.refresh();

        if (ihdlr.isKeyDown(SDL_SCANCODE_ESCAPE)) {
            exit = true;
            continue;
        }

        _stateChanged = false;
        _state->update();

        Uint32 frameTime = (Uint32)vt.currTime() - startTime;
        if (frameTime < 10)
            SDL_Delay(10 - frameTime);
    }
}

void Game::checkCollisions() {
    auto* fighter = mngr_->getHandler(ecs::hdlr::FIGHTER_HDLR);
    if (fighter == nullptr || !fighter->isAlive()) return;

    auto* fighterTr = fighter->getComponent<Transform>();
    auto* fighterGun = fighter->getComponent<Gun>();
    if (fighterTr == nullptr) return;

    auto& asteroids = mngr_->getEntities(ecs::grp::ASTEROIDS);

    // --- Balas vs Asteroides ---
    if (fighterGun != nullptr) {
        for (auto& bullet : *fighterGun) {
            if (!bullet.used) continue;
            for (auto* asteroid : asteroids) {
                if (!asteroid->isAlive()) continue;
                auto* asTr = asteroid->getComponent<Transform>();
                if (asTr == nullptr) continue;

                bool hit = Collisions::collidesWithRotation(
                    bullet.pos, (float)bullet.width, (float)bullet.height, bullet.rot,
                    asTr->getPos(), asTr->getWidth(), asTr->getHeight(), 0.0f
                );

                if (hit) {
                    bullet.used = false;
                    _au->split_astroid(asteroid);
                    sdlutils().soundEffects().at("explosion").play();
                    break;
                }
            }
        }
    }

    // Si el estado cambio durante las colisiones de balas, salir
    if (_stateChanged) return;

    // --- Caza vs Asteroides ---
    for (auto* asteroid : asteroids) {
        if (!asteroid->isAlive()) continue;
        auto* asTr = asteroid->getComponent<Transform>();
        if (asTr == nullptr) continue;

        bool hit = Collisions::collidesWithRotation(
            fighterTr->getPos(), fighterTr->getWidth(), fighterTr->getHeight(), fighterTr->getRot(),
            asTr->getPos(), asTr->getWidth(), asTr->getHeight(), 0.0f
        );

        if (hit) {
            asteroid->setAlive(false);
            sdlutils().soundEffects().at("explosion").play();
            int livesLeft = _fu->update_lives(-1);
            if (livesLeft <= 0)
                setState(GAMEOVER);
            else
                setState(NEWROUND);
            return;  // salir inmediatamente, no tocar mas entidades
        }
    }
}