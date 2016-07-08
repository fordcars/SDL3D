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

// This class is the base class of all entities in the game (objects, lights, camera, etc)

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "PhysicsBody.hpp"
#include "glm/glm.hpp"

class ObjectGeometry;
class Entity
{
private:
	using constObjectGeometryPointer = std::shared_ptr<const ObjectGeometry>;
	PhysicsBody mPhysicsBody;

public:
	Entity();
	Entity(float radius, int type);
	Entity(constObjectGeometryPointer objectGeometry, bool physicsCircularShape, int physicsType);
	virtual ~Entity();
	
	PhysicsBody& getPhysicsBody();
	const PhysicsBody& getPhysicsBody() const;
};

#endif // ENTITY_HPP