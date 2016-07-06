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

#include "Light.hpp"

Light::Light()
{
	mLightBufferIndex = 0; // Will be set when we add this light to the game
	mModifiedSinceCheck = false;

	mDiffuseColor = glm::vec3(0.0f, 0.0f, 0.0f);
	mSpecularColor = glm::vec3(0.0f, 0.0f, 0.0f);

	mPower = 60.0f;
	mIsOn = true;
}

Light::Light(glm::vec3 position, glm::vec3 diffuseColor, glm::vec3 specularColor, float power)
{
	getPhysicsBody().setPosition(position);

	mDiffuseColor = diffuseColor;
	mSpecularColor = specularColor;

	mPower = power;
}

Light::~Light()
{
	// Do nothing
}

void Light::setLightBufferIndex(int index)
{
	mLightBufferIndex = index;
}

int Light::getLightBufferIndex() const
{
	return mLightBufferIndex;
}

bool Light::wasModified()
{
	bool state = mModifiedSinceCheck;
	mModifiedSinceCheck = false;

	return state;
}

void Light::setDiffuseColor(glm::vec3 color)
{
	mDiffuseColor = color;
	mModifiedSinceCheck = true;
}

glm::vec3 Light::getDiffuseColor() const
{
	return mDiffuseColor;
}

void Light::setSpecularColor(glm::vec3 color)
{
	mSpecularColor = color;
	mModifiedSinceCheck = true;
}

glm::vec3 Light::getSpecularColor() const
{
	return mSpecularColor;
}

void Light::setPower(float power)
{
	mPower = power;
	mModifiedSinceCheck = true;
}

float Light::getPower() const
{
	return mPower;
}

void Light::turnOn()
{
	if(!mIsOn) // Not already on (this check prevents an unnecessary unform GPU buffer update)
	{
		mIsOn = true;
		mModifiedSinceCheck = true;
	}
}

void Light::turnOff()
{
	if(mIsOn) // Not already off (this check prevents an unnecessary unform GPU buffer update)
	{
		mIsOn = false;
		mModifiedSinceCheck = true;
	}
}

bool Light::isOn() const
{
	return mIsOn;
}