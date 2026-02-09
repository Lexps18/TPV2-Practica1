// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <SDL_pixels.h>

#include "../ecs/Component.h"

class Transform;

class RectangleViewer: public ecs::Component {
public:
	__CMPID_DECL__(ecs::cmp::RECTANGLEVIEWER)

	RectangleViewer();
	RectangleViewer(SDL_Color color, bool empty = false);
	virtual ~RectangleViewer();

	inline void init(SDL_Color color, bool empty = false) {
		_color = color;
		_empty = empty;
	}

	void initComponent() override;
	void render() override;
private:
	Transform *_tr;
	bool _empty;
	SDL_Color _color;

};

