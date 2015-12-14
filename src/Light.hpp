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

#ifndef LIGHT_HPP_
#define LIGHT_HPP_

#include <Entity.hpp>

#include <glm/glm.hpp>

class Light : public Entity
{
private:
	glm::vec3 mDiffuseColor; // Virtually all of the time, diffuse color and specular color will be white
	glm::vec3 mSpecularColor;

	float mPower;

public:
	Light();
	Light(glm::vec3 position, glm::vec3 diffuseColor, glm::vec3 specularColor, float power);
	~Light();

	void setDiffuseColor(glm::vec3 color);
	glm::vec3 getDiffuseColor();

	void setSpecularColor(glm::vec3 color);
	glm::vec3 getSpecularColor();

	void setPower(float power);
	float getPower();
};

#endif /* LIGHT_HPP_ */