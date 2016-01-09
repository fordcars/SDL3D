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

// This class is the parent class of all entities in the game (objects, lights, camera, etc)
// This is ALWAYS in worldspace!

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Entity
{
private:
	glm::vec3 mPosition;
	glm::vec3 mScaling;
	glm::vec3 mRotation; // Rotation angle on the x, y and z axis, in degrees

	glm::vec3 mVelocity;

public:
	Entity();
	Entity(glm::vec3 position, glm::vec3 scaling, glm::vec3 rotation, glm::vec3 velocity);
	~Entity();

	void setPosition(glm::vec3 position);
	glm::vec3 getPosition() const;

	void setScaling(glm::vec3 scaling);
	glm::vec3 getScaling() const;

	void setRotation(glm::vec3 rotation);
	glm::vec3 getRotation() const;

	void setVelocity(glm::vec3 velocity);
	glm::vec3 getVelocity() const;

	glm::mat4 getModelMatrix();

	void step();
};

#endif /* ENTITY_HPP_ */