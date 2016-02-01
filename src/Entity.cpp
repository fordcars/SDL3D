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

// Use this class to expand

#include <Entity.hpp>

Entity::Entity()
{
	// Do nothing
}

Entity::Entity(constObjectGeometryPointer objectGeometry, bool physicsCircularShape, int physicsType)
	: mPhysicsBody(objectGeometry, physicsCircularShape, physicsType)
{
	// Do nothing
}

Entity::~Entity()
{
	// Do nothing
}

PhysicsBody& Entity::getPhysicsBody()
{
	return mPhysicsBody;
}

const PhysicsBody& Entity::getPhysicsBody() const
{
	return mPhysicsBody;
}