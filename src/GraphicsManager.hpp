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

#include <string>
#include <vector>

class Light;
class GraphicsManager
{
private:
	static const int cLightSize;

	glm::ivec2 mOutputSize;
	glm::vec3 mBackgroundColor;
	GPUBuffer<float> mLightBuffer; // Uniform buffer

	// Since all lights are the sime size, we can easily manage the uniform buffer's memory
	// by having slots. Each slot has a 0-based index. When we remove lights, we create holes.
	// The next lights added will fill in those holes.

	// Vector holding the state of all vertices, false if it is free, true if it is used.
	std::vector<bool> mLightUsedBufferMap;

public:
	GraphicsManager(glm::ivec2 outputSize);
	~GraphicsManager();
	void init();

	void clearScreen();

	void setOutputSize(glm::ivec2 outputSize);
	glm::ivec2 getOutputSize();

	void setBackgroundColor(glm::vec3 color);
	glm::vec3 getBackgroundColor();

	// If we eventually add other uniform buffers, get rid of all of these methods and use a
	// cleaner, more object-oriented approach
	bool updateLightBuffer(const Light& light);
	int getNextAvailableLightIndex();

	bool addLightToBuffer(Light& light);
	bool removeLightFromBuffer(Light& light);
	bool modifyLightInBuffer(const Light& light);
};

#endif // GRAPHICS_MANAGER_HPP