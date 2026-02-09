// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include "../ecs/Component.h"

class Transform;
class Texture;

class Image: public ecs::Component {
public:

	// This line expands to the following (see the defintion of
	// __CMPID_DECL__ en ecs.h):
	//
	//    constexpr static ecs::cmpId_type id = ecs::_IMAGE
	//
	__CMPID_DECL__(ecs::cmp::IMAGE)

	Image();
	Image(const Texture *tex);
	virtual ~Image();

	void setTexture(Texture *tex) {
		_tex = tex;
	}

	void initComponent() override;
	void render() override;

private:
	Transform *_tr;
	const Texture *_tex;
};

