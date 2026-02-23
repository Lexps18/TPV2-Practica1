// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <string>
#include <cmath>
#include "GameState.h"
#include "FighterUtils.h"
#include "AsteroidsUtils.h"
#include "../sdlutils/SDLUtils.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/Texture.h"
#include "../sdlutils/macros.h"
#include "Game.h"

// ---- Helpers ----

inline void drawCenteredText(const std::string& text, int y, SDL_Color color) {
    Texture tex(sdlutils().renderer(), text,
        sdlutils().fonts().at("NES16"), color);
    float x = (sdlutils().width() - tex.width()) / 2.0f;
    tex.render(x, (float)y - tex.height() / 2.0f);
}

inline void drawHearts(int lives) {
    if (lives <= 0) return;
    auto& heartTex = sdlutils().images().at("heart");
    float size = 28.0f;
    for (int i = 0; i < lives; i++) {
        SDL_FRect dest{ 8.0f + i * (size + 4.0f), 8.0f, size, size };
        heartTex.render(dest);
    }
}

// ============================================================
// NewGameState
// ============================================================
class NewGameState : public GameState {
public:
    NewGameState(Game* game, FighterUtils* fu, AsteroidsUtils* au)
        : game_(game), fu_(fu), au_(au) {
    }
    void enter()  override {}
    void leave()  override {}
    void update() override {
        sdlutils().clearRenderer(build_sdlcolor(0x00000000));
        int cy = sdlutils().height() / 2;
        drawCenteredText("A S T E R O I D S", cy - 70, build_sdlcolor(0xffff00ff));
        drawCenteredText("Flechas izq/der: girar", cy - 20, build_sdlcolor(0x888888ff));
        drawCenteredText("W / flecha arriba: acelerar", cy + 5, build_sdlcolor(0x888888ff));
        drawCenteredText("S: disparar    P: pausa", cy + 30, build_sdlcolor(0x888888ff));
        drawCenteredText("press any key to start", cy + 75, build_sdlcolor(0xffffffff));
        sdlutils().presentRenderer();
        if (ih().keyDownEvent()) {
            fu_->reset_lives();
            game_->setState(Game::NEWROUND);
        }
    }
private:
    Game* game_; FighterUtils* fu_; AsteroidsUtils* au_;
};

// ============================================================
// NewRoundState
// ============================================================
class NewRoundState : public GameState {
public:
    NewRoundState(Game* game, FighterUtils* fu, AsteroidsUtils* au)
        : game_(game), fu_(fu), au_(au) {
    }
    void enter()  override {}
    void leave()  override {}
    void update() override {
        sdlutils().clearRenderer(build_sdlcolor(0x00000000));
        drawHearts(fu_->get_lives());
        drawCenteredText("press ENTER to start the round",
            sdlutils().height() / 2, build_sdlcolor(0xffffffff));
        sdlutils().presentRenderer();
        if (ih().isKeyDown(SDL_SCANCODE_RETURN)) {
            fu_->reset_fighter();
            au_->remove_all_asteroids();
            au_->create_asteroids(10);
            game_->setState(Game::RUNNING);
        }
    }
private:
    Game* game_; FighterUtils* fu_; AsteroidsUtils* au_;
};

// ============================================================
// RunningState
// ============================================================
class RunningState : public GameState {
public:
    RunningState(Game* game, FighterUtils* fu, AsteroidsUtils* au)
        : game_(game), fu_(fu), au_(au), _lastAsteroidTime(0) {
    }

    void enter() override {
        _lastAsteroidTime = sdlutils().virtualTimer().currTime();
    }
    void leave() override {}

    void update() override {
        if (au_->count() == 0) {
            game_->setState(Game::GAMEOVER);
            return;
        }
        if (ih().isKeyDown(SDL_SCANCODE_P)) {
            game_->setState(Game::PAUSED);
            return;
        }

        uint32_t now = sdlutils().virtualTimer().currTime();
        if (now - _lastAsteroidTime >= 5000u) {
            _lastAsteroidTime = now;
            au_->create_asteroids(1);
        }

        game_->getMngr()->update();

        game_->checkCollisions();
        // Si checkCollisions cambio el estado (vida perdida o muerte), salir
        if (game_->stateChanged()) return;

        game_->getMngr()->refresh();

        sdlutils().clearRenderer(build_sdlcolor(0x00000000));
        game_->getMngr()->render();
        drawHearts(fu_->get_lives());
        sdlutils().presentRenderer();
    }

private:
    Game* game_; FighterUtils* fu_; AsteroidsUtils* au_;
    uint32_t _lastAsteroidTime;
};

// ============================================================
// PausedState
// ============================================================
class PausedState : public GameState {
public:
    PausedState(Game* game, FighterUtils* fu, AsteroidsUtils* au)
        : game_(game), fu_(fu), au_(au) {
    }
    void enter() override { sdlutils().virtualTimer().pause(); }
    void leave() override { sdlutils().virtualTimer().resume(); }
    void update() override {
        sdlutils().clearRenderer(build_sdlcolor(0x00000000));
        int cy = sdlutils().height() / 2;
        drawCenteredText("- PAUSED -",
            cy - 80, build_sdlcolor(0xffff00ff));
        drawCenteredText("Lives: " + std::to_string(fu_->get_lives()),
            cy - 30, build_sdlcolor(0xffffffff));
        drawCenteredText("Asteroids: " + std::to_string(au_->count()),
            cy, build_sdlcolor(0xffffffff));
        drawCenteredText("Min dist: " + std::to_string((int)std::round(au_->minDistanceToFighter())),
            cy + 30, build_sdlcolor(0xffffffff));
        drawCenteredText("press any key to resume",
            cy + 80, build_sdlcolor(0x00ff00ff));
        drawHearts(fu_->get_lives());
        sdlutils().presentRenderer();
        if (ih().keyDownEvent()) game_->setState(Game::RUNNING);
    }
private:
    Game* game_; FighterUtils* fu_; AsteroidsUtils* au_;
};

// ============================================================
// GameOverState
// ============================================================
class GameOverState : public GameState {
public:
    GameOverState(Game* game, FighterUtils* fu, AsteroidsUtils* au)
        : game_(game), fu_(fu), au_(au), _won(false), _enterTime(0), _lives(0) {
    }

    void enter() override {
        _won = (au_->count() == 0);
        _enterTime = sdlutils().virtualTimer().currTime();
        // Guardar vidas AHORA antes de que el fighter pueda ser destruido
        _lives = fu_->get_lives();
        sdlutils().soundEffects().at("explosion").play();
    }
    void leave() override {}

    void update() override {
        sdlutils().clearRenderer(build_sdlcolor(0x00000000));
        int cy = sdlutils().height() / 2;

        if (_won) {
            drawCenteredText("** GAME OVER **",
                cy - 60, build_sdlcolor(0xffff00ff));
            drawCenteredText("Champion! All asteroids destroyed!",
                cy - 20, build_sdlcolor(0x00ff00ff));
        }
        else {
            drawCenteredText("** GAME OVER **",
                cy - 60, build_sdlcolor(0xff0000ff));
            drawCenteredText("You ran out of lives... Loser!",
                cy - 20, build_sdlcolor(0xff4444ff));
        }

        drawHearts(_lives);   // usar las vidas guardadas en enter(), no las del fighter

        uint32_t elapsed = sdlutils().virtualTimer().currTime() - _enterTime;
        if (elapsed > 1500u) {
            drawCenteredText("Press ENTER to play again",
                cy + 40, build_sdlcolor(0xffffffff));
            if (ih().isKeyDown(SDL_SCANCODE_RETURN))
                game_->setState(Game::NEWGAME);
        }
        sdlutils().presentRenderer();
    }

private:
    Game* game_; FighterUtils* fu_; AsteroidsUtils* au_;
    bool     _won;
    uint32_t _enterTime;
    int      _lives;   // guardado en enter() para evitar acceso a fighter muerto
};