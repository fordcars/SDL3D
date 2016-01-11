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

#include <Entity.hpp>

Entity::Entity()
	: mPosition(0.0f, 0.0f, 0.0f), mScaling(1.0f, 1.0f, 1.0f), mRotation(0.0f, 0.0f, 0.0f), mVelocity(0.0f, 0.0f, 0.0f)
{
	// Do nothing
}

Entity::Entity(glm::vec3 position, glm::vec3 scaling, glm::vec3 rotation, glm::vec3 velocity)
{
	mPosition = position;
	mScaling = scaling;
	mRotation = rotation;
}

Entity::~Entity()
{
	// Do nothing
}

void Entity::setPosition(glm::vec3 position)
{
	mPosition = position;
}

glm::vec3 Entity::getPosition() const
{
	return mPosition;
}

void Entity::setScaling(glm::vec3 scaling)
{
	mScaling = scaling;
}

glm::vec3 Entity::getScaling() const
{
	return mScaling;
}

void Entity::setRotation(glm::vec3 rotation)
{
	mRotation = rotation;
}

glm::vec3 Entity::getRotation() const
{
	return mRotation;
}

void Entity::setVelocity(glm::vec3 velocity)
{
	mVelocity = velocity;
}

glm::vec3 Entity::getVelocity() const
{
	return mVelocity;
}

glm::mat4 Entity::getModelMatrix()
{
	// Scaling * rotation * translation

	glm::mat4 translationM = glm::translate(glm::mat4(1.0f), mPosition);

	// Big chunk since we have to do x, y and z rotation manually
	glm::mat4 rotationXM = glm::rotate(translationM,
		glm::radians(mRotation.x), // Glm takes radians
		glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 rotationXYM = glm::rotate(rotationXM,
		glm::radians(mRotation.y),
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 rotationXYZM = glm::rotate(rotationXYM,
		glm::radians(mRotation.z),
		glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 modelM = glm::scale(rotationXYZM, mScaling);

	return modelM;
}

void Entity::step()
{
	if(mVelocity != glm::vec3(0.0f, 0.0f, 0.0f)) // Since it will often be like this, optimize!
		mPosition += mVelocity;
}