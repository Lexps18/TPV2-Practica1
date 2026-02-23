#pragma once
#include <array>
#include <cassert>
#include <cstdint>
#include "../ecs/Component.h"
#include "../ecs/Entity.h"
#include "Transform.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"

struct Gun : ecs::Component {
    __CMPID_DECL__(ecs::cmp::GUN)

        struct Bullet {
        bool     used = false;
        Vector2D pos;           // top-left
        Vector2D vel;
        float    rot = 0.0f; // grados, misma que el caza
        float    width = 5.0f;
        float    height = 14.0f;
    };

    static constexpr int MAX_BULLETS = 20;
    typedef std::array<Bullet, MAX_BULLETS> BulletArray;
    typedef BulletArray::iterator iterator;

    Gun() : _lastShootTime(0), _lastIdx(0) { reset(); }

    void reset() { for (auto& b : _bullets) b.used = false; }

    iterator begin() { return _bullets.begin(); }
    iterator end() { return _bullets.end(); }

    void update() override {
        float sw = (float)sdlutils().width();
        float sh = (float)sdlutils().height();

        for (auto& b : _bullets) {
            if (!b.used) continue;
            b.pos = b.pos + b.vel;
            if (b.pos.getX() < -b.width || b.pos.getX() > sw ||
                b.pos.getY() < -b.height || b.pos.getY() > sh)
                b.used = false;
        }

        if (ih().isKeyDown(SDL_SCANCODE_S)) {
            uint32_t now = sdlutils().virtualTimer().currTime();
            if (now - _lastShootTime >= 250u) {
                _lastShootTime = now;
                fire();
            }
        }
    }

    void render() override {
        const auto& tex = sdlutils().images().at("fire");
        for (const auto& b : _bullets) {
            if (!b.used) continue;
            // Renderizar sin rotacion visual para simplificar
            // (la bala va en la direccion correcta, solo el sprite es siempre vertical)
            SDL_FRect dest{
                b.pos.getX(),
                b.pos.getY(),
                b.width,
                b.height
            };
            tex.render(dest);
        }
    }

private:
    void fire() {
        auto* tr = _ent->getComponent<Transform>();
        assert(tr != nullptr);

        float r = tr->getRot();
        float w = tr->getWidth();
        float h = tr->getHeight();
        Vector2D pos = tr->getPos();
        Vector2D vel = tr->getVel();

        // Centro del caza
        Vector2D center = pos + Vector2D(w * 0.5f, h * 0.5f);

        // La punta del caza esta arriba del centro, rotada segun r
        // Vector "arriba" del caza = (0,-1) rotado r grados
        Vector2D up = Vector2D(0.0f, -1.0f).rotate(r);

        // La bala sale de la punta: centro + up * (h/2 + 2)
        Vector2D bulletCenter = center + up * (h * 0.5f + 2.0f);

        float bw = 5.0f, bh = 14.0f;
        // top-left de la bala
        Vector2D bp = bulletCenter - Vector2D(bw * 0.5f, bh * 0.5f);

        // Velocidad en la direccion del caza
        float speed = vel.magnitude() + 8.0f;
        Vector2D bv = up * speed;

        // Insertar en pool circular
        for (int i = 0; i < MAX_BULLETS; i++) {
            int idx = (_lastIdx + 1 + i) % MAX_BULLETS;
            if (!_bullets[idx].used) {
                _bullets[idx].used = true;
                _bullets[idx].pos = bp;
                _bullets[idx].vel = bv;
                _bullets[idx].rot = r;
                _bullets[idx].width = bw;
                _bullets[idx].height = bh;
                _lastIdx = idx;
                sdlutils().soundEffects().at("gunshot").play();
                return;
            }
        }
    }

    uint32_t   _lastShootTime;
    int        _lastIdx;
    BulletArray _bullets;
};