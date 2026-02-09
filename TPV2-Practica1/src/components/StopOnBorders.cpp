// This file is part of the course TPV2@UCM - Samir Genaim

#include "StopOnBorders.h"

#include <cassert>

#include "../ecs/Entity.h"
#include "../sdlutils/SDLUtils.h"
#include "Transform.h"

StopOnBorders::StopOnBorders() :
		_tr() {
}

StopOnBorders::~StopOnBorders() {
}

void StopOnBorders::initComponent() {
	_tr = _ent->getComponent<Transform>();
	assert(_tr != nullptr);
}

void StopOnBorders::update() {
	auto &pos = _tr->getPos();
	auto &vel = _tr->getVel();

	// check upper/lower borders
	if (pos.getY() < 0) {
		pos.setY(0.0f);
		vel.set(0.0f, 0.0f);
	} else if (pos.getY() + _tr->getHeight() > sdlutils().height()) {
		pos.setY(sdlutils().height() - _tr->getHeight());
		vel.set(0.0f, 0.0f);
	}
}
