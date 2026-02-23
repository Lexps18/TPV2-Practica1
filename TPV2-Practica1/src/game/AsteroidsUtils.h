// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <algorithm>
#include <cmath>
#include "AsteroidsFacade.h"
#include "../ecs/Entity.h"
#include "../ecs/EntityManager.h"
#include "../components/Transform.h"
#include "../components/Image.h"
#include "../components/ImageWithFrames.h"
#include "../components/Generations.h"
#include "../components/DisableOnCollision.h"
#include "../components/WrapAround.h"
#include "../components/TeleportOnExit.h"
#include "../components/Follow.h"
#include "../components/TowardDestination.h"
#include "../components/MaterialConsistency.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"
#include "ecs_defs.h"

class AsteroidsUtils : public AsteroidsFacade {
public:
    AsteroidsUtils(ecs::EntityManager* mngr) : mngr_(mngr) {}
    virtual ~AsteroidsUtils() {}

    void create_asteroids(int n) override {
        for (int i = 0; i < n; i++) {
            int gen = sdlutils().rand().nextInt(1, 4);
            createAsteroid(gen);
        }
    }

    void remove_all_asteroids() override {
        for (auto* a : mngr_->getEntities(ecs::grp::ASTEROIDS))
            a->setAlive(false);
    }

    void split_astroid(ecs::Entity* a) override {
        auto* tr = a->getComponent<Transform>();
        auto* gen = a->getComponent<Generations>();
        auto* mc = a->getComponent<MaterialConsistency>();
        if (tr == nullptr || gen == nullptr) { a->setAlive(false); return; }

        int g = gen->numGenerations_;
        a->setAlive(false);

        if (g > 1) {
            auto& rng = sdlutils().rand();
            Vector2D p = tr->getPos();
            Vector2D v = tr->getVel();
            float w = tr->getWidth(), h = tr->getHeight();
            int mcVal = (mc != nullptr) ? mc->getConsistency() : -1;

            for (int k = 0; k < 2; k++) {
                float r = (float)rng.nextInt(0, 360);
                Vector2D newPos = p + v.rotate(r) * 2.0f * std::max(w, h);
                Vector2D newVel = v.rotate(r) * 1.1f;
                int newGen = g - 1;

                auto* child = createBaseAsteroid(newPos, newVel, newGen);
                if (mcVal >= 0)
                    child->addComponent<MaterialConsistency>(mcVal);
            }
        }
    }

    int count() const {
        int n = 0;
        for (auto* a : mngr_->getEntities(ecs::grp::ASTEROIDS))
            if (a->isAlive()) n++;
        return n;
    }

    float minDistanceToFighter() const {
        auto* fighter = mngr_->getHandler(ecs::hdlr::FIGHTER_HDLR);
        if (fighter == nullptr) return 0.0f;
        auto* fTr = fighter->getComponent<Transform>();
        if (fTr == nullptr) return 0.0f;

        Vector2D fPos = fTr->getPos();
        float minDist = 999999.0f;
        for (auto* a : mngr_->getEntities(ecs::grp::ASTEROIDS)) {
            if (!a->isAlive()) continue;
            auto* aTr = a->getComponent<Transform>();
            if (aTr == nullptr) continue;
            float dist = (aTr->getPos() - fPos).magnitude();
            if (dist < minDist) minDist = dist;
        }
        return (minDist < 999999.0f) ? minDist : 0.0f;
    }

private:
    // Crea el cuerpo base de un asteroide (sin comportamiento ni imagen especial)
    ecs::Entity* createBaseAsteroid(Vector2D pos, Vector2D vel, int gen) {
        auto& rng = sdlutils().rand();
        // Tamanios visibles: gen1=30, gen2=50, gen3=70
        float size = 20.0f + 25.0f * (float)gen;

        auto* asteroid = mngr_->addEntity(ecs::grp::ASTEROIDS);
        asteroid->addComponent<Transform>(pos, vel, size, size, 0.0f);

        // ImageWithFrames: asteroid o asteroid_gold aleatoriamente
        asteroid->addComponent<ImageWithFrames>();

        asteroid->addComponent<Generations>(gen);
        asteroid->addComponent<DisableOnCollision>();

        // ShowAtOpposieSide (WrapAround) o TeleportOnExit aleatoriamente
        if (rng.nextInt(0, 2) == 0)
            asteroid->addComponent<WrapAround>();
        else
            asteroid->addComponent<TeleportOnExit>();

        return asteroid;
    }

    void createAsteroid(int gen) {
        auto& rng = sdlutils().rand();

        // Posicion aleatoria en los bordes
        float ax, ay;
        int border = rng.nextInt(0, 4);
        switch (border) {
        case 0: ax = (float)rng.nextInt(0, sdlutils().width());  ay = 0.0f; break;
        case 1: ax = (float)rng.nextInt(0, sdlutils().width());  ay = (float)sdlutils().height(); break;
        case 2: ax = 0.0f; ay = (float)rng.nextInt(0, sdlutils().height()); break;
        default: ax = (float)sdlutils().width(); ay = (float)rng.nextInt(0, sdlutils().height()); break;
        }

        float cx = sdlutils().width() / 2.0f + (float)rng.nextInt(-100, 101);
        float cy = sdlutils().height() / 2.0f + (float)rng.nextInt(-100, 101);

        Vector2D p(ax, ay);
        float speed = rng.nextInt(1, 11) / 10.0f;
        Vector2D v = (Vector2D(cx, cy) - p).normalize() * speed;

        auto* asteroid = createBaseAsteroid(p, v, gen);

        // Follow o TowardDestination (aleatorio, o ninguno)
        int behavior = rng.nextInt(0, 3);
        if (behavior == 1)
            asteroid->addComponent<Follow>();
        else if (behavior == 2)
            asteroid->addComponent<TowardDestination>(speed);

        // MaterialConsistency aleatoriamente
        if (rng.nextInt(0, 2) == 0)
            asteroid->addComponent<MaterialConsistency>();
    }

    ecs::EntityManager* mngr_;
};