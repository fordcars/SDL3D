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
#include "Light.hpp"

#include "glm/glm.hpp"

#include <string>
#include <vector>

class Game; // Forward declare game to avoid circular dependencies
class GraphicsManager
{
private:
	static const int cLightSize;

	Game& mGame;

	glm::vec3 mBackgroundColor;
	GPUBuffer<float> mLightBuffer;

	// Since all lights are the sime size, we can easily manage the uniform buffer's memory
	// by having slots. Each slot has a 0-based index. When we remove lights, we create holes.
	// The next lights added will fill in those holes.

	// Vector holding the state of all vertices, false if it is free, true if it is used.
	std::vector<bool> mLightUsedBufferMap;

public:
	GraphicsManager(Game& game);
	~GraphicsManager();

	void cleanGraphics();

	void setBackgroundColor(glm::vec3 color);
	glm::vec3 getBackgroundColor();

	bool modifyLightBuffer(const Light& light, int index);
	int getNextAvailableLightIndex();
	int addLight(const Light& light);
	bool modifyLight(const Light& light, int index);
	bool removeLight(int index);
};

#endif // GRAPHICS_MANAGER_HPP