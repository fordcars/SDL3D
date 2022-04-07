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

#include <Entity.hpp>
#include <ObjectGeometry.hpp>

#include <glm/glm.hpp>

class Light : public Entity
{
private:
	using constShaderPointer = std::shared_ptr<const Shader>; // Const shader

	constShaderPointer mShaderPointer;
	ObjectGeometry mObjectGeometry;
	glm::vec3 mDiffuseColor; // Virtually all of the time, diffuse color and specular color will be white
	glm::vec3 mSpecularColor;

	float mPower;
	bool mOnState; // Can turn the light on or off

public:
	Light(constShaderPointer shaderPointer, glm::vec3 position, glm::vec3 diffuseColor, glm::vec3 specularColor, float power);
	~Light() override;

	void renderDeferred(const Camera& camera,  GLuint positionTexture, GLuint normalTexture, GLuint albedoTexture);

	void setDiffuseColor(glm::vec3 color);
	glm::vec3 getDiffuseColor();

	void setSpecularColor(glm::vec3 color);
	glm::vec3 getSpecularColor();

	void setPower(float power);
	float getPower();

	void setOnState(bool onState);
	bool isOn();
};

#endif /* LIGHT_HPP */