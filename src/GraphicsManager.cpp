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

#include "GraphicsManager.hpp"

#include "Utils.hpp"
#include "Definitions.hpp"

#include "glad/glad.h"
#include <algorithm>
#include <memory>

// The size of 1 light in a GPU buffer according to the std140 layout. See modifyLightBuffer()
// You don't need the 'static' keyword here since it was already declared, like a static function.
const int GraphicsManager::cLightSize = (sizeof(float) * 4 * 3) + (sizeof(float) * 2);
// TODOOOO MOVE GL+ INITIALIZATION TO HIGHER CLASS THAN GAME (AND INITIALIZE UTILS THERE TOO!), THEN ENTITYMANAGER CALL LIGHT FUNCTION TO GIVE IT INDEX AND GRAPHICSMANAGER
// New TODO: Remove circular dependencies with Game and Engine and give EntityManager reference to GraphicsManager
GraphicsManager::GraphicsManager(glm::ivec2 outputSize)
	: mLightBuffer(GL_UNIFORM_BUFFER),
	mLightUsedBufferMap(GRAPHICS_MAX_LIGHTS, false) // Fill in the vector with false
{
	mOutputSize = outputSize;
	mBackgroundColor = glm::vec3(0.0f, 0.0f, 1.0f);
	
	// Alocate the buffer's memory
	mLightBuffer.setMutableData(GRAPHICS_MAX_LIGHTS * cLightSize, GL_DYNAMIC_DRAW);

	init();
}

GraphicsManager::~GraphicsManager()
{
	// Do nothing
}

// Initialize a few graphics stuff
void GraphicsManager::init()
{
	// Make sure the OpenGL context extends over the whole screen
	glViewport(0, 0, mOutputSize.x, mOutputSize.y);

	GLuint vertexArrayID; // VAO - vertex array object
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
}

// Call each frame
void GraphicsManager::clearScreen()
{
	// Set clear color
	glClearColor(mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, 1.0f);

	// Clear both color buffers and depth (z-indexes) buffers to have a clean buffer to render in
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// It looks like it's better to call these each frame
	glEnable(GL_DEPTH_TEST); // Enable depth test (check if z is closer to the screen than last fragement's z)
	glDepthFunc(GL_LESS); // Accept the fragment closer to the camera

	// Cull triangles which normal is not towards the camera
	// If there are holes in the model because of this, click the "invert normals" button in your 3D modeler.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GRAPHICS_RASTERIZE_FACE, GRAPHICS_RASTERIZE_MODE);
}

// Set the draw size in the framebuffer, mostly used internally
void GraphicsManager::setOutputSize(glm::ivec2 outputSize)
{
	mOutputSize = outputSize;
	glViewport(0, 0, outputSize.x, outputSize.y);
}

glm::ivec2 GraphicsManager::getOutputSize()
{
	return mOutputSize;
}


void GraphicsManager::setBackgroundColor(glm::vec3 color)
{
	mBackgroundColor = color;
}

glm::vec3 GraphicsManager::getBackgroundColor()
{
	return mBackgroundColor;
}

// Returns true on success and false on failure
// Modifies the uniform buffer with serialized data according to the std140 uniform block layout.
// Can also be used to add a light in the buffer
// The light is converted into serialized data with padding
// according to std140. It is then written to the memory. Technically, all types should be 32-bit floats,
// which is what OpenGL would want.
bool GraphicsManager::updateLightBuffer(const Light& light) // index is an int for Lua and consistency
{
	int index = light.getLightBufferIndex();

	if(index < 0 && index >= GRAPHICS_MAX_LIGHTS)
	{
		Utils::CRASH("Light uniform buffer index '" + std::to_string(index) + "' to modify out of range!" +
			" It cannot be under 0 and must be under " + std::to_string(GRAPHICS_MAX_LIGHTS) + ".");
		return false;
	}

	glm::vec3 position = light.getPhysicsBody().getPosition();
	glm::vec3 diffuseColor = light.getDiffuseColor();
	glm::vec3 specularColor = light.getSpecularColor();

	std::vector<float> data = {
		position.x, position.y, position.z, 0, // Vec3 padding required by std140
		diffuseColor.r, diffuseColor.g, diffuseColor.b, 0,
		specularColor.r, specularColor.g, specularColor.b, 0,
		light.getPower(),
		static_cast<float>(light.isOn())
	};

	mLightBuffer.modify(index * cLightSize, data);
	mLightUsedBufferMap[index] = true; // Set the index as being used if it wasn't already

	return true;
}

// Returns -1 on error
int GraphicsManager::getNextAvailableLightIndex()
{
	for(std::size_t i = 0; i < mLightUsedBufferMap.size(); i++)
		if(mLightUsedBufferMap[i] == false)
			return i;

	return -1; // If all indices are taken, out of memory!
}

// Add a light according to the GLSL std140 uniform block layout
// Returns false on error
bool GraphicsManager::addLight(Light& light)
{
	int ourIndex = getNextAvailableLightIndex();

	if(ourIndex == -1) // Failed, buffer is full!
	{
		Utils::CRASH("Light uniform buffer out of memory! Cannot add a new light. You can only have up to " +
			std::to_string(GRAPHICS_MAX_LIGHTS) + " lights.");
		return false;
	}

	// Set the index of the light to our index (here, lights are used as containers for the indices)
	// Hint: this is not OOP
	light.setLightBufferIndex(ourIndex);
	updateLightBuffer(light);

	return true;
}

// After calling this, you cannot use the light anymore and you must call addLight() in order to use it again
// "Frees" memory
bool GraphicsManager::removeLight(Light& light)
{
	int index = light.getLightBufferIndex();

	if(index < 0 && index >= GRAPHICS_MAX_LIGHTS)
	{
		Utils::CRASH("Light index to remove from uniform buffer out of range! Cannot remove.");
		return false;
	}

	mLightUsedBufferMap[index] = false; // Memory freed!
	return true;
}

// Modify a light (the light must exist in order to be modified)
// Returns true on success, false on failure
bool GraphicsManager::modifyLightInBuffer(const Light& light)
{
	int index = light.getLightBufferIndex();

	if(mLightUsedBufferMap[index] == false) // No light here! Better be safe and tell the client.
	{
		Utils::CRASH("Light to modify in the uniform buffer at the index '" + std::to_string(index) + "' does not exist!");
		return false;
	}

	updateLightBuffer(light);
	return true;
}