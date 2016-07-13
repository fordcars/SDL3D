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

#ifndef GRAPHICS_MANAGER_HPP
#define GRAPHICS_MANAGER_HPP

#include "GPUBuffer.hpp"

#include "glm/glm.hpp"
#include "glad/glad.h"

#include <cstddef> // For std::size_t
#include <string>
#include <vector>

class Light;
class GraphicsManager
{
private:
	using uniformBlockBuffer = GPUBuffer<float>;

	static const std::size_t cLightSize;

	glm::ivec2 mOutputSize;
	glm::vec3 mBackgroundColor;
	int mLightCount; // Useful for some guys
	uniformBlockBuffer mLightBuffer; // Uniform buffer

public:
	static const GLuint cLightBindingPoint;

	GraphicsManager(glm::ivec2 outputSize);
	~GraphicsManager();
	void init();
	void initBuffers();

	void clearScreen();

	void setOutputSize(glm::ivec2 outputSize);
	glm::ivec2 getOutputSize() const;

	void setBackgroundColor(glm::vec3 color);
	glm::vec3 getBackgroundColor() const;

	// If we eventually add other uniform buffers, get rid of all of these methods and use a
	// cleaner, more object-oriented approach
	int getLightCount() const;
	uniformBlockBuffer& getLightBuffer();
	bool updateLightBuffer(const Light& light);

	bool addLightToBuffer(Light& light);
	bool removeLightFromBuffer(Light& light);
	bool modifyLightInBuffer(const Light& light);
};

#endif // GRAPHICS_MANAGER_HPP