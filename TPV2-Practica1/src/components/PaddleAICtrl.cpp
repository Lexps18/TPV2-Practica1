// This file is part of the course TPV2@UCM - Samir Genaim

#include "PaddleAICtrl.h"

#include <cassert>

#include "../ecs/Entity.h"
#include "../ecs/EntityManager.h"
#include "../sdlutils/SDLUtils.h"
#include "Transform.h"

PaddleAICtrl::PaddleAICtrl() :
		_tr(nullptr), _ballTr(nullptr) {
}

PaddleAICtrl::~PaddleAICtrl() {
}

void PaddleAICtrl::initComponent() {
	_tr = _ent->getComponent<Transform>();
	assert(_tr != nullptr);

	auto mngr = _ent->getMngr();
	_ballTr = mngr->getHandler(ecs::hdlr::BALL)->getComponent<Transform>();
	assert(_ballTr != nullptr);

}

void PaddleAICtrl::update() {

	auto &vel = _tr->getVel();
	auto &pos = _tr->getPos();
	auto height = _tr->getHeight();

	if (ballComingTowardsPaddle(pos)) {
		float diff = (_ballTr->getPos().getY() + _ballTr->getHeight() / 2.0f)
				- (pos.getY() + height / 2.0f);
		if (abs(diff) < 10) {
			vel.setY(0.0f);
		} else {
			vel.setY(
					(diff > 0.0f ? 1.0f : -1.0f) * 1.0f
							* _ballTr->getVel().magnitude());
		}
	} else {
		float diff = (sdlutils().height() / 2.0f)
				- (pos.getY() + height / 2.0f);
		if (abs(diff) < 10) {
			vel.setY(0.0);
		} else {
			vel.setY((diff > 0 ? 1 : -1) * 5.0f);
		}
	}
}

bool PaddleAICtrl::ballComingTowardsPaddle(const Vector2D &pos) {

	return (pos.getX() < _ballTr->getPos().getX()
			&& _ballTr->getVel().getX() < 0)
			|| (pos.getX() > _ballTr->getPos().getX()
					&& _ballTr->getVel().getX() > 0);
}
