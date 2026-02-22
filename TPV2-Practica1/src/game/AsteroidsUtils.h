// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <algorithm>
#include "AsteroidsFacade.h"
#include "../ecs/Entity.h"
#include "../ecs/EntityManager.h"
#include "../components/Transform.h"
#include "../components/Image.h"
#include "../components/Generations.h"
#include "../components/DisableOnCollision.h"
#include "../components/WrapAround.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"
#include "ecs_defs.h"

class AsteroidsUtils : public AsteroidsFacade {
public:
    AsteroidsUtils(ecs::EntityManager* mngr) : mngr_(mngr) {}
    virtual ~AsteroidsUtils() {}

    // Crea n asteroides en los bordes de la pantalla apuntando al centro
    void create_asteroids(int n) override {
        auto& rng = sdlutils().rand();
        for (int i = 0; i < n; i++) {
            int gen = rng.nextInt(1, 4); // [1, 3]
            createAsteroid(gen);
        }
    }

    // Desactiva todos los asteroides del juego
    void remove_all_asteroids() override {
        for (auto* a : mngr_->getEntities(ecs::grp::ASTEROIDS))
            a->setAlive(false);
    }

    // Divide un asteroide en 2 sub-asteroides (gen-1), o lo elimina si gen<=1
    void split_astroid(ecs::Entity* a) override {
        auto* tr = a->getComponent<Transform>();
        auto* gen = a->getComponent<Generations>();
        if (tr == nullptr || gen == nullptr) return;

        int g = gen->numGenerations_;
        a->setAlive(false);

        if (g > 1) {
            auto& rng = sdlutils().rand();
            Vector2D p = tr->getPos();
            Vector2D v = tr->getVel();
            float w = tr->getWidth(), h = tr->getHeight();

            for (int k = 0; k < 2; k++) {
                float r = (float)rng.nextInt(0, 360);
                Vector2D newPos = p + v.rotate(r) * 2.0f * std::max(w, h);
                Vector2D newVel = v.rotate(r) * 1.1f;
                int   newGen = g - 1;
                float newSize = 10.0f + 5.0f * (float)newGen;

                auto* asteroid = mngr_->addEntity(ecs::grp::ASTEROIDS);
                asteroid->addComponent<Transform>(newPos, newVel, newSize, newSize, 0.0f);
                asteroid->addComponent<Image>(&sdlutils().images().at("asteroid"));
                asteroid->addComponent<Generations>(newGen);
                asteroid->addComponent<DisableOnCollision>();
                asteroid->addComponent<WrapAround>();
            }
        }
    }

    // Devuelve cuantos asteroides vivos hay
    int count() const {
        int n = 0;
        for (auto* a : mngr_->getEntities(ecs::grp::ASTEROIDS))
            if (a->isAlive()) n++;
        return n;
    }

private:
    void createAsteroid(int gen) {
        auto& rng = sdlutils().rand();
        float aSize = 10.0f + 5.0f * (float)gen;

        // Posicion aleatoria en los bordes
        float ax, ay;
        int border = rng.nextInt(0, 4);
        switch (border) {
        case 0: ax = (float)rng.nextInt(0, sdlutils().width());  ay = 0.0f; break;
        case 1: ax = (float)rng.nextInt(0, sdlutils().width());  ay = (float)sdlutils().height(); break;
        case 2: ax = 0.0f; ay = (float)rng.nextInt(0, sdlutils().height()); break;
        default: ax = (float)sdlutils().width(); ay = (float)rng.nextInt(0, sdlutils().height()); break;
        }

        // Posicion central con offset aleatorio
        float cx = sdlutils().width() / 2.0f + (float)rng.nextInt(-100, 101);
        float cy = sdlutils().height() / 2.0f + (float)rng.nextInt(-100, 101);

        Vector2D p(ax, ay);
        Vector2D c(cx, cy);
        float speed = rng.nextInt(1, 11) / 10.0f;
        Vector2D v = (c - p).normalize() * speed;

        auto* asteroid = mngr_->addEntity(ecs::grp::ASTEROIDS);
        asteroid->addComponent<Transform>(p, v, aSize, aSize, 0.0f);
        asteroid->addComponent<Image>(&sdlutils().images().at("asteroid"));
        asteroid->addComponent<Generations>(gen);
        asteroid->addComponent<DisableOnCollision>();
        asteroid->addComponent<WrapAround>();
    }

    ecs::EntityManager* mngr_;
};