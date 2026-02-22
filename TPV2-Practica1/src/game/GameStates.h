// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <string>
#include "GameState.h"
#include "FighterUtils.h"
#include "AsteroidsUtils.h"
#include "../sdlutils/SDLUtils.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/Texture.h"
#include "../sdlutils/macros.h"

// Forward declaration
class Game;

// Helper: dibuja texto centrado en la posicion y dada
// Crea Font y Texture temporales cada frame (sencillo, no optimo para produccion)
inline void drawCenteredText(const std::string& text, int y, SDL_Color color) {
    Texture tex(sdlutils().renderer(), text, sdlutils().fonts().at("NES16"), color);
    float x = (sdlutils().width() - tex.width()) / 2.0f;
    float fy = (float)y - tex.height() / 2.0f;
    tex.render(x, fy);
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
        sdlutils().clearRenderer();
        drawCenteredText("press any key to start a new game",
            sdlutils().height() / 2, build_sdlcolor(0xffffffff));
        sdlutils().presentRenderer();

        if (ih().keyDownEvent()) {
            fu_->reset_lives();
            game_->setState(Game::NEWROUND);
        }
    }

private:
    Game* game_;
    FighterUtils* fu_;
    AsteroidsUtils* au_;
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
        sdlutils().clearRenderer();
        drawCenteredText("press ENTER to start the round",
            sdlutils().height() / 2, build_sdlcolor(0xffffffff));
        sdlutils().presentRenderer();

        if (ih().isKeyDown(SDL_SCANCODE_RETURN)) {
            fu_->reset_fighter();
            au_->remove_all_asteroids();
            au_->create_asteroids(5);
            game_->setState(Game::RUNNING);
        }
    }

private:
    Game* game_;
    FighterUtils* fu_;
    AsteroidsUtils* au_;
};

// ============================================================
// RunningState
// ============================================================
class RunningState : public GameState {
public:
    RunningState(Game* game, FighterUtils* fu, AsteroidsUtils* au)
        : game_(game), fu_(fu), au_(au) {
    }

    void enter()  override {}
    void leave()  override {}

    void update() override {
        // Sin asteroides -> victoria
        if (au_->count() == 0) {
            game_->setState(Game::GAMEOVER);
            return;
        }

        // Pausa
        if (ih().isKeyDown(SDL_SCANCODE_P)) {
            game_->setState(Game::PAUSED);
            return;
        }

        game_->getMngr()->update();
        game_->checkCollisions();
        game_->getMngr()->refresh();

        sdlutils().clearRenderer();
        game_->getMngr()->render();

        // HUD: vidas
        drawCenteredText("Lives: " + std::to_string(fu_->get_lives()),
            20, build_sdlcolor(0xffffffff));

        sdlutils().presentRenderer();
    }

private:
    Game* game_;
    FighterUtils* fu_;
    AsteroidsUtils* au_;
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
        sdlutils().clearRenderer();

        int cy = sdlutils().height() / 2;
        drawCenteredText("Lives: " + std::to_string(fu_->get_lives()), cy - 60, build_sdlcolor(0xffffffff));
        drawCenteredText("Asteroids: " + std::to_string(au_->count()), cy - 20, build_sdlcolor(0xffffffff));
        drawCenteredText("press any key to resume", cy + 30, build_sdlcolor(0xffff00ff));

        sdlutils().presentRenderer();

        if (ih().keyDownEvent()) {
            game_->setState(Game::RUNNING);
        }
    }

private:
    Game* game_;
    FighterUtils* fu_;
    AsteroidsUtils* au_;
};

// ============================================================
// GameOverState
// ============================================================
class GameOverState : public GameState {
public:
    GameOverState(Game* game, FighterUtils* fu, AsteroidsUtils* au)
        : game_(game), fu_(fu), au_(au), msg_("") {
    }

    void enter() override {
        if (au_->count() == 0)
            msg_ = "Game Over Champion! Press ENTER";
        else
            msg_ = "Game Over Loser! Press ENTER";
    }

    void leave() override {}

    void update() override {
        sdlutils().clearRenderer();
        drawCenteredText(msg_, sdlutils().height() / 2, build_sdlcolor(0xff3232ff));
        sdlutils().presentRenderer();

        if (ih().isKeyDown(SDL_SCANCODE_RETURN)) {
            game_->setState(Game::NEWGAME);
        }
    }

private:
    Game* game_;
    FighterUtils* fu_;
    AsteroidsUtils* au_;
    std::string     msg_;
};