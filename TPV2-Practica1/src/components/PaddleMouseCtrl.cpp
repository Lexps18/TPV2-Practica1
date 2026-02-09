// This file is part of the course TPV2@UCM - Samir Genaim

#include "PaddleMouseCtrl.h"

#include "../ecs/Entity.h"
#include "../sdlutils/InputHandler.h"
#include "Transform.h"

PaddleMouseCtrl::PaddleMouseCtrl() :
		_tr(nullptr) {
}

PaddleMouseCtrl::~PaddleMouseCtrl() {
}

void PaddleMouseCtrl::initComponent() {
	_tr = _ent->getComponent<Transform>();
	assert(_tr != nullptr);
}

void PaddleMouseCtrl::update() {
	auto &ihdlr = ih();

	auto &pos = _tr->getPos();

	if (ihdlr.mouseMotionEvent()) {
		pos.setY(ihdlr.getMousePos().second);
	}
}
