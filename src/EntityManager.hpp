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

#ifndef ENTITY_MANAGER_HPP_
#define ENTITY_MANAGER_HPP_

#include <Object.hpp>
#include <Light.hpp>
#include <Camera.hpp>

#include <memory>
#include <vector>

class EntityManager
{
public: // Public typedefs
	typedef std::shared_ptr<Object> objectPointer;
	typedef std::shared_ptr<Light> lightPointer;

	typedef std::vector<objectPointer> objectVector; // Vector containing shared pointers
	typedef std::vector<lightPointer> lightVector;

private:

	Camera mGameCamera; // The main camera for the game. Whatever this camera sees will be displayed on the screen.

	objectVector mObjects;
	lightVector mLights;

public:
	EntityManager();
	~EntityManager();

	Camera& getGameCamera();

	void addObject(objectPointer object);
	objectVector& getObjects();

	void addLight(lightPointer light);
	lightVector& getLights();

	void step();
	void render();
};

#endif /* ENTITY_MANAGER_HPP_ */