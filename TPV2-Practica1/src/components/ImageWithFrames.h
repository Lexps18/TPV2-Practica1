#pragma once
#include "../ecs/Component.h"
#include "../ecs/Entity.h"
#include "Transform.h"
#include "../sdlutils/SDLUtils.h"
#include "../sdlutils/Texture.h"

// Spritesheet: 6 columnas x 5 filas = 30 frames
// Paso entre frames: 89px horizontal, 101px vertical
// Tamano del frame visible: 74x84 px

struct ImageWithFrames : ecs::Component {
    __CMPID_DECL__(ecs::cmp::IMAGEWITHFRAMES)

        static constexpr int COLS = 6;
    static constexpr int ROWS = 5;
    static constexpr int N_FRAMES = COLS * ROWS; // 30
    static constexpr int STEP_X = 89;   // paso horizontal entre frames
    static constexpr int STEP_Y = 101;  // paso vertical entre frames
    static constexpr int FRAME_W = 74;   // ancho visible del frame
    static constexpr int FRAME_H = 84;   // alto visible del frame

    ImageWithFrames() : _frame(0), _lastTime(0) {
        _texKey = (sdlutils().rand().nextInt(0, 2) == 0)
            ? "asteroid" : "asteroid_gold";
    }

    void initComponent() override {
        _frame = sdlutils().rand().nextInt(0, N_FRAMES);
        _lastTime = sdlutils().virtualTimer().currTime();
    }

    void update() override {
        uint32_t now = sdlutils().virtualTimer().currTime();
        if (now - _lastTime >= 50u) {
            _lastTime = now;
            _frame = (_frame + 1) % N_FRAMES;
        }
    }

    void render() override {
        auto* tr = _ent->getComponent<Transform>();
        if (tr == nullptr) return;

        const auto& tex = sdlutils().images().at(_texKey);

        int col = _frame % COLS;
        int row = _frame / COLS;

        SDL_FRect src{
            (float)(col * STEP_X),
            (float)(row * STEP_Y),
            (float)FRAME_W,
            (float)FRAME_H
        };
        SDL_FRect dest{
            tr->getPos().getX(),
            tr->getPos().getY(),
            tr->getWidth(),
            tr->getHeight()
        };

        tex.render(src, dest);
    }

private:
    int         _frame;
    uint32_t    _lastTime;
    std::string _texKey;
};