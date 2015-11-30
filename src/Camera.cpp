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

	mPosition = glm::vec3(0, 0, 0);
	mTarget = glm::vec4(0, 0, 0, 1); // 1 for position
	mUpVector = glm::vec3(0, 1, 0);

	mFieldOfViewX = 90.0f;
	mAspectRatio = 4/3;
	mNearClippingPlane = 0.1f;
	mFarClippingPlane = 100.0f;
}

void Camera::setPosition(glm::vec3 position)
{
	mPosition = position;
}

glm::vec3 Camera::getPosition()
{
	return mPosition;
}

void Camera::setTarget(glm::vec4 target) // vec4 since this can be a position or a direction
{
	mTarget = target;
}

void Camera::setUpVector(glm::vec3 upVector)
{
	mUpVector = upVector;
}

void Camera::setFieldOfView(float fieldOfViewX) // In degrees
{
	mFieldOfViewX = fieldOfViewX;
}

void Camera::setAspectRatio(float aspectRatio)
{
	mAspectRatio = aspectRatio;
}

void Camera::updateMatrices() // Call after you are done setting up the camera
{
	glm::vec3 vec3Target;

	if(mTarget.w==1) // Is a position
		vec3Target = glm::vec3(mTarget); // Doesn't cast, it creates a new one
	else if(mTarget.w==0)
		vec3Target = glm::vec3(mTarget) + mPosition; // Is a direction

	// Calculate vertical field of view (glm::perspective()'s input)
	float radFOVX = glm::radians(mFieldOfViewX);
	float radFOVY = 2 * atan( tan(radFOVX / 2) / mAspectRatio); // Glm takes radians

	mViewMatrix = glm::lookAt(mPosition, vec3Target, mUpVector);
	mProjectionMatrix = glm::perspective(radFOVY, mAspectRatio, mNearClippingPlane, mFarClippingPlane);
}

glm::mat4 Camera::getViewMatrix()
{
	return mViewMatrix;
}

glm::mat4 Camera::getProjectionMatrix()
{
	return mProjectionMatrix;
}