// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include "FighterFacade.h"
#include "../ecs/Entity.h"
#include "../ecs/EntityManager.h"
#include "../components/Transform.h"
#include "../components/Image.h"
#include "../components/DeAcceleration.h"
#include "../components/FighterControl.h"
#include "../components/Gun.h"
#include "../components/Health.h"
#include "../components/WrapAround.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Vector2D.h"
#include "ecs_defs.h"

class FighterUtils : public FighterFacade {
public:
    FighterUtils(ecs::EntityManager* mngr) : mngr_(mngr) {}
    virtual ~FighterUtils() {}

    void create_fighter() override {
        auto* fighter = mngr_->addEntity(ecs::grp::FIGHTER);

        float fw = 40.0f, fh = 40.0f;
        float fx = (sdlutils().width() - fw) / 2.0f;
        float fy = (sdlutils().height() - fh) / 2.0f;

        fighter->addComponent<Transform>(
            Vector2D(fx, fy), Vector2D(0.0f, 0.0f), fw, fh, 0.0f
        );
        fighter->addComponent<Image>(&sdlutils().images().at("fighter"));
        fighter->addComponent<DeAcceleration>(0.99f);
        fighter->addComponent<FighterControl>(0.2f, 3.0f);
        fighter->addComponent<Gun>();
        fighter->addComponent<Health>(3);
        fighter->addComponent<WrapAround>();

        mngr_->setHandler(ecs::hdlr::FIGHTER_HDLR, fighter);
    }

    void reset_fighter() override {
        auto* fighter = mngr_->getHandler(ecs::hdlr::FIGHTER_HDLR);
        if (fighter == nullptr) return;

        fighter->setAlive(true);
        auto* tr = fighter->getComponent<Transform>();
        if (tr != nullptr) {
            float fw = tr->getWidth(), fh = tr->getHeight();
            tr->getPos() = Vector2D(
                (sdlutils().width() - fw) / 2.0f,
                (sdlutils().height() - fh) / 2.0f
            );
            tr->getVel() = Vector2D(0.0f, 0.0f);
            tr->setRot(0.0f);
        }
        auto* gun = fighter->getComponent<Gun>();
        if (gun != nullptr) gun->reset();
    }

    void reset_lives() override {
        auto* fighter = mngr_->getHandler(ecs::hdlr::FIGHTER_HDLR);
        if (fighter == nullptr) return;
        auto* health = fighter->getComponent<Health>();
        if (health != nullptr) health->lives_ = 3;
    }

    int update_lives(int n) override {
        auto* fighter = mngr_->getHandler(ecs::hdlr::FIGHTER_HDLR);
        if (fighter == nullptr) return 0;
        auto* health = fighter->getComponent<Health>();
        if (health == nullptr) return 0;
        health->lives_ += n;
        return health->lives_;
    }

    int get_lives() const {
        auto* fighter = mngr_->getHandler(ecs::hdlr::FIGHTER_HDLR);
        if (fighter == nullptr) return 0;
        auto* health = fighter->getComponent<Health>();
        return (health != nullptr) ? health->lives_ : 0;
    }

    bool is_alive() const {
        auto* fighter = mngr_->getHandler(ecs::hdlr::FIGHTER_HDLR);
        return (fighter != nullptr && fighter->isAlive());
    }

private:
    ecs::EntityManager* mngr_;
};