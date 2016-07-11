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

#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Entity.hpp"

#include "glm/glm.hpp"

class Light : public Entity
{
private:
	// Index of this light in the light uniform block buffer (int for Lua and consistency)
	// It is set when the light is added to the game
	int mLightBufferIndex;

	// Will be true if it was modified this frame. Needed since lights have to be modified in the GPU buffer.
	bool mModifiedSinceCheck;
	glm::vec3 mPositionAtLastCheck; // Since the position is store in physics body

	glm::vec3 mDiffuseColor; // Virtually all of the time, diffuse color and specular color will be white
	glm::vec3 mSpecularColor;

	float mPower;
	bool mIsOn; // Can turn the light on or off

public:
	Light();
	Light(glm::vec3 position, glm::vec3 diffuseColor, glm::vec3 specularColor, float power);
	~Light() override;

	void setLightBufferIndex(int index);
	int getLightBufferIndex() const;
	bool wasModified();

	void setDiffuseColor(glm::vec3 color);
	glm::vec3 getDiffuseColor() const;

	void setSpecularColor(glm::vec3 color);
	glm::vec3 getSpecularColor() const;

	void setPower(float power);
	float getPower() const;

	void turnOn();
	void turnOff();
	bool isOn() const;
};

#endif // LIGHT_HPP