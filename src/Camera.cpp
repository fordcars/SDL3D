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

#include <Camera.hpp>
#include <math.h>

Camera::Camera() // Constructor
{
	init();
}

Camera::Camera(float fieldOfView, float aspectRatio)
{
	init();

	mFieldOfViewX = fieldOfView;
	mAspectRatio = aspectRatio;
}

Camera::~Camera()
{
	// Do nothing
}

// Private
void Camera::init() // Called when constructed, private
{
	// Default values
	mViewMatrix = glm::mat4(1.0f); // Identity matrix
	mProjectionMatrix = glm::mat4(1.0f);

	mDirection = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // 1 for position
	mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);

	mFieldOfViewX = 90.0f;
	mAspectRatio = 4/3;
	mNearClippingPlane = 0.1f;
	mFarClippingPlane = 100.0f;
}

void Camera::setDirection(glm::vec4 direction) // vec4 since this can be a position or a direction
{
	mDirection = direction;
}

glm::vec4 Camera::getDirection()
{
	return mDirection;
}

void Camera::setUpVector(glm::vec3 upVector)
{
	mUpVector = upVector;
}

glm::vec3 Camera::getUpVector()
{
	return mUpVector;
}

void Camera::setFieldOfView(float fieldOfViewX) // In degrees
{
	mFieldOfViewX = fieldOfViewX;
}

void Camera::setAspectRatio(float aspectRatio)
{
	mAspectRatio = aspectRatio;
}

glm::mat4 Camera::getViewMatrix() const
{
	glm::vec3 position = getPosition();
	glm::vec3 vec3Direction;

	if(mDirection.w==1) // Is a point
		vec3Direction = glm::vec3(mDirection); // Doesn't cast, it creates a new one
	else if(mDirection.w==0)
		vec3Direction = glm::vec3(mDirection) + position; // Is a vector

	glm::mat4 viewMatrix = glm::lookAt(position, vec3Direction, mUpVector);

	return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const
{
	// Calculate vertical field of view (glm::perspective()'s input)
	float radFOVX = glm::radians(mFieldOfViewX);
	float radFOVY = 2 * atan( tan(radFOVX / 2) / mAspectRatio); // Glm takes radians
	
	glm::mat4 projectionMatrix = glm::perspective(radFOVY, mAspectRatio, mNearClippingPlane, mFarClippingPlane);

	return projectionMatrix;
}