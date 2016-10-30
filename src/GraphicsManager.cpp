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

// Light buffer: When we add a light to the buffer, it is always added to the end of it.
// When we remove a light, the last light of the buffer is moved into the hole we've created (if we did).
// This guarantees contiguousness, but not order, which is fine in our case. This makes the shader cleaner.

#include "GraphicsManager.hpp"

#include "Light.hpp"
#include "Utils.hpp"
#include "Definitions.hpp"

#include "Shader.hpp"

#include <algorithm>
#include <memory>

// The size of 1 light in a GPU buffer according to the std140 layout. See modifyLightBuffer()
// You don't need the 'static' keyword here since it was already declared, like a static function.
const std::size_t GraphicsManager::cLightSize = 16 * sizeof(float);

// Public
const GLuint GraphicsManager::cLightBindingPoint = 0;

GraphicsManager::GraphicsManager(glm::ivec2 outputSize)
	: mLightBuffer(GL_UNIFORM_BUFFER)
{
	mOutputSize = outputSize;
	mBackgroundColor = glm::vec3(0.0f, 0.0f, 1.0f);
	mLightCount = 0;

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

	initBuffers();
}

void GraphicsManager::initBuffers()
{
	// Alocate the buffer's memory
	mLightBuffer.setMutableData(GRAPHICS_MAX_LIGHTS * cLightSize, GL_DYNAMIC_DRAW);

	// Bind the buffer to the binding point
	// Information: how do we bind a buffer to a uniform block? We don't.
	// Instead, we bind a buffer to a binding point (between 0 and a maximum) and
	// bind the uniform block to this same binding point.
	mLightBuffer.bind(); // Bind manually to target since we are calling a gl function directly
	glBindBufferRange(mLightBuffer.getTarget(), cLightBindingPoint, mLightBuffer.getID(), 0, mLightBuffer.getSize());
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

glm::ivec2 GraphicsManager::getOutputSize() const
{
	return mOutputSize;
}

void GraphicsManager::setBackgroundColor(glm::vec3 color)
{
	mBackgroundColor = color;
}

glm::vec3 GraphicsManager::getBackgroundColor() const
{
	return mBackgroundColor;
}

int GraphicsManager::getLightCount() const
{
	return mLightCount;
}

GraphicsManager::uniformBlockBuffer& GraphicsManager::getLightBuffer()
{
	return mLightBuffer;
}

// Returns true on success and false on failure
// Modifies the uniform buffer with serialized data according to the std140 uniform block layout.
// Can also be used to add a light in the buffer
// The light is converted into serialized data with padding
// according to std140. It is then written to the memory. 
// Keep in mind the padding is only to align the data.
// So, the padding of each data is dependent on what follows next.
// For example, a single vec3 followed by a float would not require padding
// since the vec3 would be at the first address and the float
// requires a 4-byte alignement, which a vec3 (3 * 4 bytes = 12)
// is compliant (the float would be at the address 12, 12%4 = 0).
bool GraphicsManager::updateLightBuffer(const Light& light) // index is an int for Lua and consistency
{
	int index = light.getLightBufferIndex();

	if(index < 0 && index >= GRAPHICS_MAX_LIGHTS)
	{
		Utils::CRASH("Light uniform buffer index '" + std::to_string(index) + "' to update out of range!" +
			" It cannot be under 0 and must be under " + std::to_string(GRAPHICS_MAX_LIGHTS) + ".");
		return false;
	}

	glm::vec3 position = light.getPhysicsBody().getPosition() * PHYSICS_PIXELS_PER_METER;
	glm::vec3 diffuseColor = light.getDiffuseColor();
	glm::vec3 specularColor = light.getSpecularColor();

	std::vector<float> data = {
		position.x, position.y, position.z, 0.0f, // Vec3 padding required by std140 for alignment
		diffuseColor.r, diffuseColor.g, diffuseColor.b, 0.0f,
		specularColor.r, specularColor.g, specularColor.b, // No padding necessary here
		light.getPower(),
		static_cast<float>(light.isOn()), // Making this a real GLboolean is too complicated for me, make it a float
		0.0f, // Padding to make the size of the struct a multiple of 4 floats, as required by std140
		0.0f,
		0.0f
	};
	
	mLightBuffer.modify(index * cLightSize, data);
	return true;
}

// Add a light according to the GLSL std140 uniform block layout
// Returns false on error
bool GraphicsManager::addLightToBuffer(Light& light)
{
	int ourIndex = mLightCount; // Add it to the end of the buffer

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

	mLightCount++;
	return true;
}

// After calling this, you cannot use the light anymore and you must call addLight() in order to use it again
// "Frees" memory
bool GraphicsManager::removeLightFromBuffer(Light& light)
{
	int index = light.getLightBufferIndex();

	if(index < 0 && index >= GRAPHICS_MAX_LIGHTS)
	{
		Utils::CRASH("Light at index '" + std::to_string(index) + "' to remove from uniform buffer"
			" out of range! Cannot remove.");
		return false;
	} else if(index >= mLightCount)
	{
		Utils::CRASH("Light at index '" + std::to_string(index) + "' to remove does not exist! Remember, the index is 0-based.");
		return false;
	}

	// Not the last light of the buffer (also true when it is the last light in the buffer)
	if(index != mLightCount-1)
	{
		// Copy the last light of the buffer into the "hole"

		// Read the last light
		std::vector<float> lastLight =  mLightBuffer.read(cLightSize * (mLightCount - 1), cLightSize);

		// Write it to the hole
		mLightBuffer.modify(index * cLightSize, lastLight);
	}

	mLightCount--; // Now we have one less light, essentially "removing" the last light
	return true;
}

// Modify a light (the light must exist in order to be modified)
// Returns true on success, false on failure
bool GraphicsManager::modifyLightInBuffer(const Light& light)
{
	int index = light.getLightBufferIndex();

	// No light here! Better be safe and tell the client.
	if(index >= mLightCount || index < 0)
	{
		Utils::CRASH("Light at index '" + std::to_string(index) + "' to modify does not exist! "
			"Remember, the index is 0-based.");
		return false;
	}

	updateLightBuffer(light);
	return true;
}
