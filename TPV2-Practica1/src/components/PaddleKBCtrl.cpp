// This file is part of the course TPV2@UCM - Samir Genaim

#include "PaddleKBCtrl.h"

#include "../ecs/Entity.h"
#include "../sdlutils/InputHandler.h"
#include "Transform.h"

PaddleKBCtrl::PaddleKBCtrl() :
		_tr(), //
		_up(SDL_SCANCODE_UP), //
		_down(SDL_SCANCODE_DOWN), //
		_stop(SDL_SCANCODE_LEFT), //
		_speed(10.0f) //
{
}

PaddleKBCtrl::~PaddleKBCtrl() {
}

void PaddleKBCtrl::initComponent() {
	_tr = _ent->getComponent<Transform>();
	assert(_tr != nullptr);
}

void PaddleKBCtrl::update() {
	auto &ihdlr = ih();

	auto &vel = _tr->getVel();

	if (ihdlr.keyDownEvent()) {
		if (ihdlr.isKeyDown(_up)) {
			vel.setY(-_speed);
		} else if (ihdlr.isKeyDown(_down)) {
			vel.setY(_speed);
		} else if (ihdlr.isKeyDown(_stop)) {
			vel.setY(0.0f);
		}
	}
}
