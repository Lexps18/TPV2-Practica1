// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <SDL_scancode.h>

#include "../ecs/Component.h"

class Transform;

class PaddleKBCtrl: public ecs::Component {
public:

	__CMPID_DECL__(ecs::cmp::PADDLEKBCTRL)

	PaddleKBCtrl();
	virtual ~PaddleKBCtrl();

	void initComponent() override;
	void update() override;

	inline void setKeys(SDL_Scancode up, SDL_Scancode down, SDL_Scancode stop) {
		_up = up;
		_down = down;
		_stop = stop;
	}

	inline void setSpeed(float speed) {
		_speed = speed;
	}

private:
	Transform *_tr;
	SDL_Scancode _up;
	SDL_Scancode _down;
	SDL_Scancode _stop;
	float _speed;
};

