// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <vector>

#include "Component.h"
#include "ecs.h"

namespace ecs {

/*
 * A class that represents a collection of components.
 *
 */

class Entity {
public:
	Entity(ecs::grpId_t gId, EntityManager *mngr) :
			_mngr(mngr), //
			_cmps(), //
			_currCmps(), //
			_alive(),  //
			_gId(gId) //
	{
		// We reserve the a space for the maximum number of
		// components. This way we avoid resizing the vector.
		//
		_currCmps.reserve(ecs::maxComponentId);
	}

	// we delete the copy constructor/assignment because it is
	// not clear how to copy the components
	//
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;

	// Exercise: define move constructor/assignment for class Entity

	// Destroys the entity
	//
	virtual ~Entity() {

		// we delete all available components
		//
		for (auto c : _currCmps)
			delete c;
	}

	// Returns the manager to which this entity belongs
	inline EntityManager* getMngr() {
		return _mngr;
	}

	// Setting the state of the entity (alive or dead)
	//
	inline void setAlive(bool alive) {
		_alive = alive;
	}

	// Returns the state of the entity (alive o dead)
	//
	inline bool isAlive() {
		return _alive;
	}

	// Updating  an entity simply calls the update of all
	// components
	//
	void update() {
		auto n = _currCmps.size();
		for (auto i = 0u; i < n; i++)
			_currCmps[i]->update();
	}

	// Rendering an entity simply calls the render of all
	// components
	//
	void render() {
		auto n = _currCmps.size();
		for (auto i = 0u; i < n; i++)
			_currCmps[i]->render();
	}

	// Adds a component. It receives the type T (to be created), and the
	// list of arguments (if any) to be passed to the constructor.
	//
	template<typename T, typename ...Ts>
	inline T* addComponent(Ts &&... args) {

		// the component id
		constexpr cmpId_t cId = cmpId<T>;
		static_assert(cId < ecs::maxComponentId);

		// delete the current component, if any
		//
		removeComponent<T>();

		// create, initialise and install the new component
		//
		Component *c = new T(std::forward<Ts>(args)...);
		c->setContext(this);
		c->initComponent();
		_cmps[cId] = c;
		_currCmps.push_back(c);

		// return it to the user so i can be initialised if needed
		return static_cast<T*>(c);
	}

	// Removes the component T
	//
	template<typename T>
	inline void removeComponent() {

		// the component id
		constexpr cmpId_t cId = cmpId<T>;
		static_assert(cId < ecs::maxComponentId);

		if (_cmps[cId] != nullptr) {

			// find the element that is equal tocmps_[cId] (returns an iterator)
			//
			auto iter = std::find(_currCmps.begin(), _currCmps.end(),
					_cmps[cId]);

			// must have such a component
			assert(iter != _currCmps.end());

			// and then remove it
			_currCmps.erase(iter);

			// destroy it
			//
			delete _cmps[cId];

			// remove the pointer
			//
			_cmps[cId] = nullptr;
		}
	}

	// Returns the component that corresponds to T, casting it
	// to T*. The casting is done just for ease of use, to avoid casting
	// outside.
	//
	template<typename T>
	inline T* getComponent() {

		// the component id
		constexpr cmpId_t cId = cmpId<T>;
		static_assert(cId < ecs::maxComponentId);

		return static_cast<T*>(_cmps[cId]);
	}

	// return true if there is a componentT
	//
	template<typename T>
	inline bool hasComponent() {

		// the component id
		constexpr cmpId_t cId = cmpId<T>;
		static_assert(cId < ecs::maxComponentId);

		return _cmps[cId] != nullptr;
	}

	// returns the entity's group 'gId'
	//
	inline ecs::grpId_t groupId() {
		return _gId;
	}


private:
	// the fields currCmps_ can be removed, and instead we can traverse cmps_
	// and process non-null elements. We keep it because sometimes the order
	// in which the components are executed is important

	EntityManager *_mngr;
	std::array<Component*, maxComponentId> _cmps;
	std::vector<Component*> _currCmps;
	bool _alive;
	ecs::grpId_t _gId;
};

} // end of name space
