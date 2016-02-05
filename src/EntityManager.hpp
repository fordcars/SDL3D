//// Copyright 2016 Carl Hewett
////
//// This file is part of SDL3D.
////
//// SDL3D is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// SDL3D is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include <Object.hpp>
#include <Light.hpp>
#include <Camera.hpp>

#include <Box2D.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <cstddef> // For std::size_t

class EntityManager
{
public: // Public aliases
	using objectPointer = std::shared_ptr<Object>;
	using lightPointer = std::shared_ptr<Light>;

	using objectVector = std::vector<objectPointer>; // Vector containing shared pointers
	using lightVector = std::vector<lightPointer>;

private:
	Camera mGameCamera; // The main camera for the game. Whatever this camera sees will be displayed on the screen.

	objectVector mObjects;
	lightVector mLights;

	b2World mPhysicsWorld;
	float mPhysicsTimePerStep; // In seconds
	int mPhysicsVelocityIterations;
	int mPhysicsPositionIterations;

public:
	EntityManager(glm::vec2 gravity, float physicsTimePerStep);
	~EntityManager();

	void setPhysicsTimePerStep(float time);
	float getPhysicsTimePerStep();

	Camera& getGameCamera();

	bool addObject(objectPointer object);
	objectPointer removeObject(std::size_t index);
	bool removeObject(objectPointer object);
	objectVector& getObjects();

	bool addLight(lightPointer light);
	lightPointer removeLight(std::size_t index);
	bool removeLight(lightPointer light);
	lightVector& getLights();

	void step(float divider);
	void render();
};

#endif /* ENTITY_MANAGER_HPP */