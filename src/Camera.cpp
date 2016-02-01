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

#include <PhysicsBody.hpp>

#include <math.h>

Camera::Camera() // Constructor
{
	// Default values
	mViewMatrix = glm::mat4(1.0f); // Identity matrix
	mProjectionMatrix = glm::mat4(1.0f);

	mDirection = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f); // 0 for direction
	mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);

	mFieldOfViewX = 90.0f;
	mAspectRatio = 4 / 3;

	const PhysicsBody& physicsBody = getPhysicsBody();
	mNearClippingPlane = static_cast<float>(physicsBody.getPixelsPerMeter()) * 0.05f; // Relative to pixels per meter
	mFarClippingPlane = static_cast<float>(physicsBody.getPixelsPerMeter()) * 100.0f; // 100 meters max
}

Camera::~Camera()
{
	// Do nothing
}

// vec4 since this can be a position or a direction
// If it is a position, it is in meters
void Camera::setDirection(glm::vec4 direction)
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
	const PhysicsBody& physicsBody = getPhysicsBody();

	glm::vec3 position = physicsBody.getPosition() * static_cast<float>(physicsBody.getPixelsPerMeter());
	glm::vec3 vec3Direction;

	if(mDirection.w==1) // Is a position
	{
		int pixelsPerMeter = getPhysicsBody().getPixelsPerMeter();

		vec3Direction = glm::vec3(
			mDirection.x * pixelsPerMeter,
			mDirection.y * pixelsPerMeter,
			mDirection.z * pixelsPerMeter);
	} else if(mDirection.w==0)
	{
		vec3Direction = glm::vec3(mDirection) + position; // Is a vector
	}

	glm::mat4 viewMatrix = glm::lookAt(position, vec3Direction, mUpVector);

	return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const
{
	// Calculate vertical field of view (glm::perspective()'s input)
	float radFOVX = glm::radians(mFieldOfViewX); // Glm takes radians
	float radFOVY = 2 * atan( tan(radFOVX / 2) / mAspectRatio);
	
	glm::mat4 projectionMatrix = glm::perspective(radFOVY, mAspectRatio, mNearClippingPlane, mFarClippingPlane);

	return projectionMatrix;
}// TODOD NEAR CLIIIPPING WTF + BOX2D WORLD SIZE