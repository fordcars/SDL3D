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

// This class will hold onto all entities, but it will NOT take ownership!

#include <EntityManager.hpp>
#include <Utils.hpp>
#include <Definitions.hpp>
#include <ShadedObject.hpp>

#include <algorithm> // For finding in vector
#include <string>

#include <Box2D/Box2D.h>

const unsigned EntityManager::mDeferredTextureCount;

EntityManager::EntityManager(glm::vec2 gravity, float physicsTimePerStep)
	: mPhysicsWorld(b2Vec2(gravity.x, gravity.y)) // Quick type conversion shhhh
{
	mDeferredFramebuffer = 0;
	*mDeferredTextures = {0};
	mDeferredDepthbuffer = 0;

	// Defaults
	mPhysicsTimePerStep = physicsTimePerStep;
	mPhysicsVelocityIterations = 6;
	mPhysicsPositionIterations = 2;

	mGameCamera.getPhysicsBody().addToWorld(&mPhysicsWorld); // Add it to the world
}

EntityManager::~EntityManager()
{
	glDeleteTextures(mDeferredTextureCount, mDeferredTextures);
}

void EntityManager::initDeferredRendering()
{
	// Create framebuffer
	glGenFramebuffers(1, &mDeferredFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mDeferredFramebuffer);

	// Add depth buffer
	glGenRenderbuffers(1, &mDeferredDepthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDeferredDepthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, DEFAULT_GAME_WINDOW_WIDTH, DEFAULT_GAME_WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDeferredDepthbuffer);

	// Generate textures
	glGenTextures(3, mDeferredTextures);

	// Position
	glBindTexture(GL_TEXTURE_2D, mDeferredTextures[0]);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,                          // Level
		GL_RGB,                     // Internal format
		DEFAULT_GAME_WINDOW_WIDTH,
		DEFAULT_GAME_WINDOW_HEIGHT,
		0,                          // Border
		GL_RGB,
		GL_FLOAT,                   // Data format
		nullptr                     // Data (0s)
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mDeferredTextures[0], 0);

	// Normal
	glBindTexture(GL_TEXTURE_2D, mDeferredTextures[1]);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,                          // Level
		GL_RGB,                     // Internal format
		DEFAULT_GAME_WINDOW_WIDTH,
		DEFAULT_GAME_WINDOW_HEIGHT,
		0,                          // Border
		GL_RGB,
		GL_FLOAT,                   // Data format
		nullptr                     // Data (0s)
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, mDeferredTextures[1], 0);

	// Albedo
	glBindTexture(GL_TEXTURE_2D, mDeferredTextures[2]);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,                          // Level
		GL_RGB,                     // Internal format
		DEFAULT_GAME_WINDOW_WIDTH,
		DEFAULT_GAME_WINDOW_HEIGHT,
		0,                          // Border
		GL_RGB,
		GL_UNSIGNED_BYTE,           // Data format
		nullptr                     // Data (0s)
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, mDeferredTextures[2], 0);

	// Set draw buffers
	GLenum drawBuffers[mDeferredTextureCount] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};
	glDrawBuffers(mDeferredTextureCount, drawBuffers);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Utils::CRASH("Could not initialized deferred rendering framebuffer!");
}

Camera& EntityManager::getGameCamera()
{
	return mGameCamera;
}

// Returns false on failure
bool EntityManager::addObject(objectPointer object) // Give it a shared pointer
{
	if(std::find(mObjects.begin(), mObjects.end(), object) == mObjects.end()) // Does not already exist
	{
		mObjects.push_back(object);
		// If we remove this object, it will remain in the physics world until it gets destroyed!
		object->getPhysicsBody().addToWorld(&mPhysicsWorld);
		return true;
	} else
	{
		std::string objectGeometryName = object->getObjectGeometry()->getName();
		Utils::CRASH("This object (constructed from '" + objectGeometryName + "') was already added in entity manager! Cannot add again!");
		return false;
	}
}

// Removes an object from the manager. Does not delete them if they are still referenced somewhere, logically (shared pointers).
// Returns the removed object, or an empty pointer on error.
EntityManager::objectPointer EntityManager::removeObject(std::size_t index)
{
	if(index+1 <= mObjects.size()) // Check if vector subscript out of range, I really hate runtime errors
	{
		objectPointer removedItem = mObjects[index];

		objectVector::iterator iterator = mObjects.begin() + index; // We need to do this for std
		mObjects.erase(iterator);

		removedItem->getPhysicsBody().removeFromWorld();
		return removedItem;
	} else
	{
		Utils::CRASH("Cannot remove object at index '" + std::to_string(index) +
			"', the index is higher than the amount of objects!");
		return objectPointer();
	}
}

// Removes a specific object
bool EntityManager::removeObject(objectPointer object)
{
	objectVector::iterator found = std::find(mObjects.begin(), mObjects.end(), object);

	if(found != mObjects.end()) // If it is found
	{
		object->getPhysicsBody().removeFromWorld();
		mObjects.erase(found); // Remove it
		return true;
	} else
	{
		Utils::CRASH("Cannot remove object pointer; it is not in the manager!");
		return false;
	}
}

EntityManager::objectVector& EntityManager::getObjects()
{
	return mObjects;
}


bool EntityManager::addLight(lightPointer light) // Give it an actual object
{
	if(std::find(mLights.begin(), mLights.end(), light) == mLights.end()) // Does not already exist
	{
		mLights.push_back(light);
		light->getPhysicsBody().addToWorld(&mPhysicsWorld);

		return true;
	} else
	{
		Utils::CRASH("This light was already added in entity manager! Cannot add again!");
		return false;
	}
}

EntityManager::lightPointer EntityManager::removeLight(std::size_t index)
{
	if(index + 1 > mLights.size())
	{
		lightPointer removedItem = mLights[index];

		lightVector::iterator iterator = mLights.begin() + index;
		mLights.erase(iterator);

		removedItem->getPhysicsBody().removeFromWorld();
		return removedItem;
	}
	else
	{
		Utils::CRASH("Cannot remove light at index '" + std::to_string(index) +
			"'; the index is higher than the amount of lights!");
		return lightPointer(); // Return an empty pointer
	}
}

bool EntityManager::removeLight(lightPointer light)
{
	lightVector::iterator found = std::find(mLights.begin(), mLights.end(), light);

	if(found != mLights.end()) // If it is found
	{
		light->getPhysicsBody().removeFromWorld();
		mLights.erase(found); // Remove it
		return true;
	} else
	{
		Utils::CRASH("Cannot remove light pointer; it is not in the manager!");
		return false;
	}
}

EntityManager::lightVector& EntityManager::getLights()
{
	return mLights;
}

// Set the number of seconds elapsed per frame (will be under 0 most of the time)
// Allows us to do slow motion!
void EntityManager::setPhysicsTimePerStep(float time)
{
	mPhysicsTimePerStep = time;
}

float EntityManager::getPhysicsTimePerStep()
{
	return mPhysicsTimePerStep;
}

// Steps all entities
// Divider will divide the step time, useful for calling this function multiple times per frame
void EntityManager::step(float divider)
{
	float time = mPhysicsTimePerStep / divider;

	for(auto &object : mObjects)
		object->getPhysicsBody().step(time);

	for(auto &light : mLights)
		light->getPhysicsBody().step(time);

	mGameCamera.getPhysicsBody().step(time);

	mPhysicsWorld.Step(time, mPhysicsVelocityIterations, mPhysicsPositionIterations);
}

void EntityManager::render() // Renders all entities that can be rendered
{
	// First pass - shaded objets
	glBindFramebuffer(GL_FRAMEBUFFER, mDeferredFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear textures before writing to them

	for(objectVector::iterator it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		Object *object = &(*(*it));
		ShadedObject *shaded = dynamic_cast<ShadedObject *>(object);
		
		if(shaded != nullptr)
			shaded->render(mGameCamera);
	}

	// Second pass - other objects
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	for(objectVector::iterator it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		Object *object = &(*(*it));
		ShadedObject *shaded = dynamic_cast<ShadedObject *>(object);

		if(shaded == nullptr)
			object->render(mGameCamera);
	}

	// Third pass - lights
	for(lightVector::iterator it = mLights.begin(); it != mLights.end(); ++it)
	{
		(*it)->renderDeferred(mGameCamera, mDeferredTextures[0], mDeferredTextures[1], mDeferredTextures[2]);
	}
}