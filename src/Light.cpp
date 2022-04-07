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

#include <Light.hpp>

static const ObjectGeometry::vec3Vector LIGHT_VERTS = {
	glm::vec3(-1.0f, 1.0f, 0.0f),  // Top left
	glm::vec3(1.0f, 1.0f, 0.0f),   // Top right
	glm::vec3(-1.0f, -1.0f, 0.0f), // Bottom left
	glm::vec3(1.0f, -1.0f, 0.0f)   // Bottom right
};

static const ObjectGeometry::vec2Vector LIGHT_UVs = {
	glm::vec2(0.0f, 1.0f),   // Top left
	glm::vec2(1.0f, 1.0f),   // Top right
	glm::vec2(0.0f, 0.0f),   // Bottom left
	glm::vec2(1.0f, 0.0f)    // Bottom right
};

static const ObjectGeometry::uintVector LIGHT_INDICES = {
	0, 2, 1,
	2, 3, 1
};

Light::Light(constShaderPointer shaderPointer, glm::vec3 position, glm::vec3 diffuseColor, glm::vec3 specularColor, float power)
	: mObjectGeometry("lightGeometry", LIGHT_INDICES, LIGHT_VERTS, LIGHT_UVs, LIGHT_VERTS)
{
	getPhysicsBody().setPosition(position);

	mShaderPointer = shaderPointer;
	mDiffuseColor = diffuseColor;
	mSpecularColor = specularColor;

	mPower = power;
}

Light::~Light()
{
	// Do nothing
}

void Light::renderDeferred(const Camera &camera)
{

}

void Light::setDiffuseColor(glm::vec3 color)
{
	mDiffuseColor = color;
}

glm::vec3 Light::getDiffuseColor()
{
	return mDiffuseColor;
}

void Light::setSpecularColor(glm::vec3 color)
{
	mSpecularColor = color;
}

glm::vec3 Light::getSpecularColor()
{
	return mSpecularColor;
}

void Light::setPower(float power)
{
	mPower = power;
}

float Light::getPower()
{
	return mPower;
}

void Light::setOnState(bool onState)
{
	mOnState = onState;
}

bool Light::isOn()
{
	return mOnState;
}