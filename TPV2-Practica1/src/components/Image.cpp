// This file is part of the course TPV2@UCM - Samir Genaim

#include "Image.h"

#include <cassert>

#include "../ecs/Entity.h"
#include "../sdlutils/macros.h"
#include "../sdlutils/Texture.h"
#include "Transform.h"

Image::Image() :
		_tr(), _tex() {
}

Image::Image(const Texture *tex) :
		_tr(), _tex(tex) {
}

Image::~Image() {
}

void Image::initComponent() {
	_tr = _ent->getComponent<Transform>();
	assert(_tr != nullptr);
}

void Image::render() {

	SDL_FRect dest = build_sdlfrect(_tr->getPos(), _tr->getWidth(),
			_tr->getHeight());

	assert(_tex != nullptr);
	_tex->render(dest, _tr->getRot());

}
