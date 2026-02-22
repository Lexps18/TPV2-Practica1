#pragma once
#include <array>
#include <cmath>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include "../ecs/Component.h"
#include "../ecs/Entity.h"
#include "../ecs/EntityManager.h"
#include "Transform.h"
#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Gun : ecs::Component {

    // ---- Estructura de bala (pool interno) ----
    struct Bullet {
        bool used = false;
        Vector2D pos;
        Vector2D vel;
        int width = 5;
        int height = 20;
        float rot = 0.0f;
    };

    constexpr static uint8_t _max_bullets = 20;
    typedef std::array<Bullet, _max_bullets> bullets_array_t;
    typedef bullets_array_t::iterator iterator;

    __CMPID_DECL__(ecs::cmp::GUN)

        Gun() : _lastShootTime(0), _lastUsedIdx(0) {
        reset();
    }

    // Marca todas las balas como no usadas
    void reset() {
        for (auto& b : _bullets)
            b.used = false;
    }

    // Iteradores para recorrer el array desde fuera (colisiones)
    iterator begin() { return _bullets.begin(); }
    iterator end() { return _bullets.end(); }

    // Update: mover balas activas + disparar con S
    void update() override {
        // Mover balas activas
        for (auto& b : _bullets) {
            if (b.used)
                b.pos = b.pos + b.vel;
        }

        // Disparar si se pulsa S y ha pasado suficiente tiempo (0.25s = 250ms)
        auto& ihdlr = ih();
        if (ihdlr.isKeyDown(SDL_SCANCODE_S)) {
            uint32_t now = sdlutils().virtualTimer().currTime();
            if (now - _lastShootTime >= 250u) {
                _lastShootTime = now;
                fireFromFighter();
            }
        }
    }

    // Render: dibujar todas las balas activas
    void render() override {
        auto& tex = sdlutils().images().at("fire");
        for (auto& b : _bullets) {
            if (!b.used) continue;
            SDL_FRect dest{
                b.pos.getX(),
                b.pos.getY(),
                (float)b.width,
                (float)b.height
            };
            tex.render(dest, b.rot);
        }
    }

private:
    // Calcula posicion/velocidad de la bala y la inserta en el pool
    void fireFromFighter() {
        auto* tr = _ent->getComponent<Transform>();
        assert(tr != nullptr);

        Vector2D p = tr->getPos();
        Vector2D v = tr->getVel();
        float    r = tr->getRot();
        float    w = tr->getWidth();
        float    h = tr->getHeight();

        int bw = 5;
        int bh = 20;

        // Calculo segun el enunciado
        Vector2D c = p + Vector2D(w / 2.0f, h / 2.0f);
        Vector2D bp = c - Vector2D((float)(bw / 2), h / 2.0f + 5.0f + (float)bh).rotate(r)
            - Vector2D((float)(bw / 2), (float)(bh / 2));
        Vector2D bv = Vector2D(0.0f, -1.0f).rotate(r) * (v.magnitude() + 5.0f);
        float    br = Vector2D(0.0f, -1.0f).angle(bv);

        shoot(bp, bv, bw, bh, br);

        sdlutils().soundEffects().at("gunshot").play();
    }

    // Busqueda circular de bala libre e inicializacion
    void shoot(Vector2D p, Vector2D vel, int w, int h, float r) {
        for (uint8_t i = 0; i < _max_bullets; i++) {
            uint8_t idx = (_lastUsedIdx + 1 + i) % _max_bullets;
            if (!_bullets[idx].used) {
                _bullets[idx].used = true;
                _bullets[idx].pos = p;
                _bullets[idx].vel = vel;
                _bullets[idx].width = w;
                _bullets[idx].height = h;
                _bullets[idx].rot = r;
                _lastUsedIdx = idx;
                return;
            }
        }
        // Si no hay bala libre, no se dispara
    }

    uint32_t        _lastShootTime;
    uint8_t         _lastUsedIdx;
    bullets_array_t _bullets;
};