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

#include <EntityManager.hpp>

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

Camera& EntityManager::getGameCamera()
{
	return mGameCamera;
}

void EntityManager::addObject(objectPointer object) // Give it a shared pointer
{
	mObjects.push_back(object);
}

EntityManager::objectVector& EntityManager::getObjects()
{
	return mObjects;
}

void EntityManager::addLight(lightPointer light) // Give it an actual object
{
	mLights.push_back(light);
}

EntityManager::lightVector& EntityManager::getLights()
{
	return mLights;
}

void EntityManager::step() // Steps all entities
{
	mGameCamera.step();

	for(objectVector::iterator it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		(*it)->step(); // it is a pointer (iterator) pointing to a smart pointer
	}

	for(lightVector::iterator it = mLights.begin(); it != mLights.end(); ++it)
	{
		(*it)->step();
	}
}

void EntityManager::render() // Renders all entities that can be rendered
{
	for(objectVector::iterator it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		(*it)->render(mGameCamera);
	}
}