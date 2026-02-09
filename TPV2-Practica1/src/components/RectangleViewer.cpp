// This file is part of the course TPV2@UCM - Samir Genaim

#include "RectangleViewer.h"

#include <cassert>
#include "../ecs/Entity.h"
#include "../sdlutils/macros.h"
#include "../sdlutils/SDLUtils.h"
#include "Transform.h"

RectangleViewer::RectangleViewer() :
		RectangleViewer(build_sdlcolor(0xffffffff), false) {
}

RectangleViewer::RectangleViewer(SDL_Color color, bool empty) :
		_tr(nullptr), //
		_empty(empty), //
		_color(color) {
}

RectangleViewer::~RectangleViewer() {
}

void RectangleViewer::initComponent() {
	_tr = _ent->getComponent<Transform>();
	assert(_tr != nullptr);
}

void RectangleViewer::render() {
	auto &pos = _tr->getPos();
	auto width = _tr->getWidth();
	auto height = _tr->getHeight();

	SDL_SetRenderDrawColor(sdlutils().renderer(), COLOREXP(_color));
	SDL_FRect rect = build_sdlfrect(pos.getX(), pos.getY(), width, height);

	if (_empty) {
		SDL_RenderRect(sdlutils().renderer(), &rect);
	} else {
		SDL_RenderFillRect(sdlutils().renderer(), &rect);
	}
}
