//// Copyright 2015 Carl Hewett
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

#ifndef POINTLIGHT_HPP_
#define POINTLIGHT_HPP_

#include <glm/glm.hpp>

class PointLight
{
private:
	glm::vec3 mPosition;
	glm::vec3 mDiffuseColor; // Virtually all of the time, diffuse color and specular color will be white
	glm::vec3 mSpecularColor;

	float mPower;

public:
	PointLight(glm::vec3 position, glm::vec3 diffuseColor, glm::vec3 specularColor, float power);
	~PointLight();
};

#endif /* POINTLIGHT_HPP_ */