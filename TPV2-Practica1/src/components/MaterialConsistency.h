#pragma once
#include "../ecs/Component.h"
#include "../ecs/Entity.h"
#include "../sdlutils/SDLUtils.h"

// Cada 5 segundos tiene un 10% de probabilidad de perder 1 unidad de consistencia.
// Cuando llega a 0, el asteroide muere.

struct MaterialConsistency : ecs::Component {
    __CMPID_DECL__(ecs::cmp::MATERIALCONSISTENCY)

        MaterialConsistency() : _consistency(0), _lastCheckTime(0) {
        // Valor aleatorio entre 10 y 100
        _consistency = sdlutils().rand().nextInt(10, 101);
    }

    MaterialConsistency(int consistency)
        : _consistency(consistency), _lastCheckTime(0) {
    }

    void initComponent() override {
        _lastCheckTime = sdlutils().virtualTimer().currTime();
    }

    void update() override {
        if (_consistency <= 0) {
            _ent->setAlive(false);
            return;
        }

        // Cada 5 segundos (5000ms) comprobamos con 10% de probabilidad
        uint32_t now = sdlutils().virtualTimer().currTime();
        if (now - _lastCheckTime >= 5000u) {
            _lastCheckTime = now;
            // 10% de probabilidad de perder 1 unidad
            if (sdlutils().rand().nextInt(0, 10) == 0) {
                _consistency--;
                if (_consistency <= 0)
                    _ent->setAlive(false);
            }
        }
    }

    int getConsistency() const { return _consistency; }

private:
    int      _consistency;
    uint32_t _lastCheckTime;
};