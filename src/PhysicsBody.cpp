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

#include <PhysicsBody.hpp>

#include <Utils.hpp>
#include <Definitions.hpp>

#include <glm/glm.hpp>

#include <cstddef> // For std::size_t

PhysicsBody::PhysicsBody(const ObjectGeometry& objectGeometry, int type)
{
	mBody = nullptr; // nullptr is a keyword

	switch(type)
	{
	case PHYSICS_BODY_IGNORED:
		break;
		
	case PHYSICS_BODY_STATIC:
		mBody = createStaticBodyFromObjectGeometry(objectGeometry);
		break;

	case  PHYSICS_BODY_DYNAMIC:
		break;

	default:
		Utils::CRASH("No valid physics body type specified!");
		break;
	}
}

PhysicsBody::~PhysicsBody()
{
	// Do nothing
}

// Static
b2Body* PhysicsBody::createStaticBodyFromObjectGeometry(const ObjectGeometry& objectGeometry)
{
	std::vector<b2Vec2> positions2D = get2DBoundingBoxFromObjectGeometry(objectGeometry, true);



	b2PolygonShape shape;
	shape.Set(positions2D.data(), positions2D.size());
}

// Static
b2Body* PhysicsBody::createDynamicBodyFromObjectGeometry(const ObjectGeometry& objectGeometry)
{
}

// Static
// Copies the data into a new vector, quite heavy because of this
// Will project as topdown if topdown is set to true
// Returns a b2Vec2 to assure compability with Box2D
std::vector<b2Vec2> PhysicsBody::get2DBoundingBoxFromObjectGeometry(const ObjectGeometry& objectGeometry, bool topdown)
{
	ObjectGeometry::uintVector indices = objectGeometry.getIndexBuffer().readData();
	ObjectGeometry::vec3Vector positions3D = objectGeometry.getPositionBuffer().readData();

	std::vector<b2Vec2> positions2D(indices.size()); // Same number of elements

	// Convert polygons to 2D
	for(std::size_t i = 0; i < indices.size(); i++)
	{
		unsigned int vertexIndex = indices[i]; // The index in the positions vector of this vertex
		glm::vec3 vertexPosition3D = positions3D[vertexIndex];

		if(topdown)
			positions2D[i] = b2Vec2(vertexPosition3D.x, vertexPosition3D.z);
		else
			positions2D[i] = b2Vec2(vertexPosition3D.x, vertexPosition3D.y);
	}



	for(std::size_t i = 0; i < positions2D.size(); i++)
	{
		// TODOOOOO this and then multiply with matrix and stuff


	return positions2D;
}