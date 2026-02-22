// This file is part of the course TPV2@UCM - Samir Genaim

#include "Game.h"

#include <iostream>
#include "../components/Transform.h"
#include "../components/Image.h"
#include "../components/DeAcceleration.h"
#include "../components/FollowMouse.h"
#include "../components/Gun.h"
#include "../components/Generations.h"
#include "../components/Health.h"
#include "../components/DisableOnCollision.h"
#include "../ecs/EntityManager.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"
#include "ecs_defs.h"

Game::Game() :
    mngr_(nullptr),
    _state(nullptr),
    _running_state(nullptr),
    _paused_state(nullptr),
    _newgame_state(nullptr),
    _newround_state(nullptr),
    _gameover_state(nullptr)
{
}

Game::~Game() {
    delete mngr_;

    if (InputHandler::HasInstance())
        InputHandler::Release();

    if (SDLUtils::HasInstance())
        SDLUtils::Release();
}

bool Game::init() {
    // Inicializar SDLUtils
    if (!SDLUtils::Init("Asteroids", 800, 600,
        "resources/config/asteroid.resources.json")) {
        std::cerr << "Error inicializando SDLUtils" << std::endl;
        return false;
    }

    // Inicializar InputHandler
    if (!InputHandler::Init()) {
        std::cerr << "Error inicializando InputHandler" << std::endl;
        return false;
    }

    sdlutils().showCursor();
    return true;
}

void Game::initGame() {
    // Crear el manager
    mngr_ = new ecs::EntityManager();

    // --- Crear el CAZA ---
    auto* fighter = mngr_->addEntity(ecs::grp::FIGHTER);

    float fw = 40.0f, fh = 40.0f;
    float fx = (sdlutils().width() - fw) / 2.0f;
    float fy = (sdlutils().height() - fh) / 2.0f;

    fighter->addComponent<Transform>(
        Vector2D(fx, fy),   // posicion central
        Vector2D(0.0f, 0.0f), // velocidad inicial
        fw, fh,             // tamanio
        0.0f                // rotacion
    );
    fighter->addComponent<Image>(&sdlutils().images().at("fighter"));
    fighter->addComponent<DeAcceleration>(0.99f);
    fighter->addComponent<FollowMouse>();
    fighter->addComponent<Gun>();
    fighter->addComponent<Health>(3);

    // Registrar el caza como handler
    mngr_->setHandler(ecs::hdlr::FIGHTER_HDLR, fighter);

    // --- Crear ASTEROIDES iniciales ---
    int numAsteroids = 5;
    for (int i = 0; i < numAsteroids; i++) {
        auto& rng = sdlutils().rand();

        // Generaciones aleatorias entre 1 y 3
        int gen = rng.nextInt(1, 4); // [1, 3]

        // Tamano segun generaciones
        float aSize = 10.0f + 5.0f * gen;

        // Posicion aleatoria en los bordes
        float ax, ay;
        int border = rng.nextInt(0, 4); // 0=arriba, 1=abajo, 2=izq, 3=der
        switch (border) {
        case 0: ax = (float)rng.nextInt(0, sdlutils().width());  ay = 0.0f; break;
        case 1: ax = (float)rng.nextInt(0, sdlutils().width());  ay = (float)sdlutils().height(); break;
        case 2: ax = 0.0f; ay = (float)rng.nextInt(0, sdlutils().height()); break;
        default: ax = (float)sdlutils().width(); ay = (float)rng.nextInt(0, sdlutils().height()); break;
        }

        // Posicion objetivo: zona central + offset aleatorio
        float cx = sdlutils().width() / 2.0f + (float)rng.nextInt(-100, 101);
        float cy = sdlutils().height() / 2.0f + (float)rng.nextInt(-100, 101);

        Vector2D p(ax, ay);
        Vector2D c(cx, cy);

        // Velocidad hacia el centro
        float speed = rng.nextInt(1, 11) / 10.0f; // [0.1, 1.0]
        Vector2D v = (c - p).normalize() * speed;

        auto* asteroid = mngr_->addEntity(ecs::grp::ASTEROIDS);
        asteroid->addComponent<Transform>(p, v, aSize, aSize, 0.0f);
        asteroid->addComponent<Image>(&sdlutils().images().at("asteroid"));
        asteroid->addComponent<Generations>(gen);
        asteroid->addComponent<DisableOnCollision>();
    }
}

void Game::setState(State s) {
    // Por ahora no tenemos estados concretos implementados,
    // dejamos el cuerpo preparado para la practica 2
    // _state->leave();
    // switch(s) { ... }
    // _state->enter();
}

void Game::start() {
    bool exit = false;
    auto& ihdlr = ih();

    sdlutils().virtualTimer().resetTime();

    while (!exit) {
        Uint32 startTime = sdlutils().virtualTimer().currTime();

        ihdlr.refresh();

        if (ihdlr.isKeyDown(SDL_SCANCODE_ESCAPE)) {
            exit = true;
            continue;
        }

        mngr_->update();

        checkCollisions();

        mngr_->refresh();

        sdlutils().clearRenderer();
        mngr_->render();
        sdlutils().presentRenderer();

        Uint32 frameTime = sdlutils().virtualTimer().currTime() - startTime;
        if (frameTime < 10)
            SDL_Delay(10 - frameTime);
    }
}

void Game::checkCollisions() {
    // Obtener el caza
    auto* fighter = mngr_->getHandler(ecs::hdlr::FIGHTER_HDLR);
    if (fighter == nullptr || !fighter->isAlive()) return;

    auto* fighterTr = fighter->getComponent<Transform>();
    auto* fighterHealth = fighter->getComponent<Health>();
    auto* fighterGun = fighter->getComponent<Gun>();

    if (fighterTr == nullptr) return;

    auto& asteroids = mngr_->getEntities(ecs::grp::ASTEROIDS);

    // --- Colision BALAS vs ASTEROIDES ---
    if (fighterGun != nullptr) {
        for (auto& bullet : *fighterGun) {
            if (!bullet.used) continue;

            for (auto* asteroid : asteroids) {
                if (!asteroid->isAlive()) continue;

                auto* asTr = asteroid->getComponent<Transform>();
                if (asTr == nullptr) continue;

                // Colision simple AABB
                float bx = bullet.pos.getX(), by = bullet.pos.getY();
                float bw = (float)bullet.width, bh = (float)bullet.height;
                float ax = asTr->getPos().getX(), ay = asTr->getPos().getY();
                float aw = asTr->getWidth(), ah = asTr->getHeight();

                bool collides =
                    bx < ax + aw && bx + bw > ax &&
                    by < ay + ah && by + bh > ay;

                if (collides) {
                    // Desactivar bala
                    bullet.used = false;

                    // Dividir o destruir asteroide
                    auto* gen = asteroid->getComponent<Generations>();
                    int g = (gen != nullptr) ? gen->numGenerations_ : 0;

                    // Desactivar asteroide actual
                    asteroid->setAlive(false);

                    // Crear sub-asteroides si gen > 1
                    if (g > 1) {
                        auto& rng = sdlutils().rand();
                        Vector2D ap = asTr->getPos();
                        Vector2D av = asTr->getVel();
                        float aw2 = asTr->getWidth(), ah2 = asTr->getHeight();

                        for (int k = 0; k < 2; k++) {
                            int r = rng.nextInt(0, 360);
                            Vector2D newPos = ap + av.rotate((float)r) * 2.0f * std::max(aw2, ah2);
                            Vector2D newVel = av.rotate((float)r) * 1.1f;

                            int newGen = g - 1;
                            float newSize = 10.0f + 5.0f * newGen;

                            auto* newAsteroid = mngr_->addEntity(ecs::grp::ASTEROIDS);
                            newAsteroid->addComponent<Transform>(newPos, newVel, newSize, newSize, 0.0f);
                            newAsteroid->addComponent<Image>(&sdlutils().images().at("asteroid"));
                            newAsteroid->addComponent<Generations>(newGen);
                            newAsteroid->addComponent<DisableOnCollision>();
                        }
                    }

                    sdlutils().soundEffects().at("explosion").play();
                    break; // esta bala ya colisiono, siguiente bala
                }
            }
        }
    }

    // --- Colision CAZA vs ASTEROIDES ---
    for (auto* asteroid : asteroids) {
        if (!asteroid->isAlive()) continue;

        auto* asTr = asteroid->getComponent<Transform>();
        if (asTr == nullptr) continue;

        float fx2 = fighterTr->getPos().getX(), fy2 = fighterTr->getPos().getY();
        float fw2 = fighterTr->getWidth(), fh2 = fighterTr->getHeight();
        float ax2 = asTr->getPos().getX(), ay2 = asTr->getPos().getY();
        float aw2 = asTr->getWidth(), ah2 = asTr->getHeight();

        bool collides =
            fx2 < ax2 + aw2 && fx2 + fw2 > ax2 &&
            fy2 < ay2 + ah2 && fy2 + fh2 > ay2;

        if (collides) {
            // Desactivar asteroide
            asteroid->setAlive(false);

            // Quitar una vida al caza
            if (fighterHealth != nullptr) {
                fighterHealth->lives_--;
                if (fighterHealth->lives_ <= 0) {
                    fighter->setAlive(false);
                }
            }
            break;
        }
    }
}