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
#include <algorithm>
#include <cmath>

PhysicsBody::PhysicsBody(constObjectGeometryPointer objectGeometry, bool circularShape, int type)
{
	mBody = nullptr; // nullptr is a keyword

	mObjectGeometry = objectGeometry;
	mCircularShape = circularShape;
	mRadius = 0.0f;
	mType = type;

	if(type != PHYSICS_BODY_IGNORED &&
		type != PHYSICS_BODY_STATIC &&
		type != PHYSICS_BODY_DYNAMIC)
	{
		Utils::CRASH("No valid physics body type specified!");
	}

	if(type != PHYSICS_BODY_IGNORED)
		calculateShape();
}

// Will copy, but will not touch any world
PhysicsBody::PhysicsBody(const PhysicsBody& other)
{
	mBody = nullptr;

	mObjectGeometry = other.mObjectGeometry;
	mCircularShape = other.mCircularShape;
	mRadius = other.mRadius;
	mType = other.mType;

	b2BlockAllocator block;

	if(mCircularShape)
		block.Allocate(sizeof(b2CircleShape));
	else
		block.Allocate(sizeof(b2PolygonShape));

	mShape = b2ShapeUniquePointer(other.mShape->Clone(&block));
}

PhysicsBody::~PhysicsBody()
{
	// Do nothing
}

// Static
// Gives ownership
PhysicsBody::b2ShapeUniquePointer PhysicsBody::createShapeFromObjectGeometry(const ObjectGeometry& objectGeometry,
	bool generateCircular)
{
	// b2Vec2 is a float32 point/vector
	std::vector<b2Vec2> positions2D = get2DCoordsObjectGeometry(objectGeometry, true);

	if(generateCircular)
	{
		b2Vec2 centroid = getCentroid(positions2D);
		float radius = getCircleRadius(positions2D, centroid);
		
		b2CircleShape* circleShape = new b2CircleShape();
		circleShape->m_p = centroid;
		circleShape->m_radius = radius;

		return b2ShapeUniquePointer(circleShape); // Moves the unique ptr!
	} else
	{
		b2Vec2Vector box = monotoneChainConvexHull(positions2D);
		b2PolygonShape* polygonShape = new b2PolygonShape(); // Will be converted into a smart pointer
		polygonShape->Set(box.data(), positions2D.size());

		return b2ShapeUniquePointer(polygonShape);
	}
}

PhysicsBody::b2ShapeUniquePointer PhysicsBody::createShapeFromRadius(float radius)
{
	b2CircleShape* circleShape = new b2CircleShape();
	circleShape->m_p = b2Vec2(0.0f, 0.0f);
	circleShape->m_radius = radius;

	return b2ShapeUniquePointer(circleShape); // Moves the unique ptr!
}

// Static
// Heavy!
// Copies the data into a new vector
// Will project as topdown if topdown is set to true (will always be x,y coords, though)
// Returns a b2Vec2 to assure compability with Box2D
PhysicsBody::b2Vec2Vector PhysicsBody::get2DCoordsObjectGeometry(const ObjectGeometry& objectGeometry, bool topdown)
{
	ObjectGeometry::uintVector indices = objectGeometry.getIndexBuffer().readData();
	ObjectGeometry::vec3Vector positions3D = objectGeometry.getPositionBuffer().readData();

	b2Vec2Vector positions2D(indices.size()); // Same number of elements

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

	return positions2D;
}

// Static
// From https://en.wikibooks.org/wiki/Algorithm_Implementation/Geometry/Convex_hull/Monotone_chain
// Heavy!
// Copies and returns a new vector
PhysicsBody::b2Vec2Vector PhysicsBody::monotoneChainConvexHull(b2Vec2Vector points2D)
{
	int numberOfPoints = points2D.size(), hullIndex = 0;
	b2Vec2Vector hull(2 * numberOfPoints); // *2 to make sure

	// Sort points
	// Give a lamda since b2Vec2 don't define the >operator.
	std::sort(points2D.begin(), points2D.end(), [](b2Vec2 first, b2Vec2 second) // True if first should be before second
	{
		// Only compares the x-coords in this case. If the x-coords are the same, take the one with the smallest y-coord.
		return first.x < second.x || (first.x == second.x && first.y < second.y);
	});

	// Build lower hull
	for(int i = 0; i < numberOfPoints; ++i)
	{
		while(hullIndex >= 2 && cross(hull[hullIndex - 2], hull[hullIndex - 1], points2D[i]) <= 0)
			hullIndex--;
		hull[hullIndex++] = points2D[i];
	}

	// Build upper hull
	for(int i = numberOfPoints-2, t = hullIndex+1; i >= 0; i--)
	{
		while(hullIndex >= t && cross(hull[hullIndex - 2], hull[hullIndex - 1], points2D[i]) <= 0)
			hullIndex--;
		hull[hullIndex++] = points2D[i];
	}

	hull.resize(hullIndex);
	return hull; // Note: C++ automatically moves values instead of copying when returning
}

// Static
// From http://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon
b2Vec2 PhysicsBody::getCentroid(const b2Vec2Vector& points2D)
{
	std::size_t pointCount = points2D.size();

	b2Vec2 centroid(0.0f, 0.0f);
	float signedArea = 0.0;
	float x0 = 0.0f; // Current vertex X
	float y0 = 0.0f; // Current vertex Y
	float x1 = 0.0f; // Next vertex X
	float y1 = 0.0f; // Next vertex Y
	float a = 0.0f;  // Partial signed area

	// For all vertices except last
	std::size_t i = 0;
	for(i = 0; i<pointCount - 1; ++i)
	{
		x0 = points2D[i].x;
		y0 = points2D[i].y;
		x1 = points2D[i + 1].x;
		y1 = points2D[i + 1].y;
		a = x0*y1 - x1*y0;
		signedArea += a;
		centroid.x += (x0 + x1)*a;
		centroid.y += (y0 + y1)*a;
	}

	// Do last vertex separately to avoid performing an expensive
	// modulus operation in each iteration.
	x0 = points2D[i].x;
	y0 = points2D[i].y;
	x1 = points2D[0].x;
	y1 = points2D[0].y;
	a = x0*y1 - x1*y0;
	signedArea += a;
	centroid.x += (x0 + x1)*a;
	centroid.y += (y0 + y1)*a;

	signedArea *= 0.5f;
	centroid.x /= (6.0f * signedArea);
	centroid.y /= (6.0f * signedArea);

	return centroid;
}

// Static
float PhysicsBody::getCircleRadius(b2Vec2Vector points2D, b2Vec2 centroid)
{
	// Find the farthest point, the distance will be the radius
	float farthestDistanceSquared = 0.0f;

	for(b2Vec2 point : points2D)
	{
		float distanceSquared = (point.x * point.x) + (point.y * point.y);

		if(distanceSquared > farthestDistanceSquared)
			farthestDistanceSquared = distanceSquared;
	}

	return sqrt(farthestDistanceSquared);
}

// Static
float PhysicsBody::cross(const b2Vec2& O, const b2Vec2& A, const b2Vec2& B)
{
	return (A.x - O.x)*(B.y - O.y) - (A.y - O.y)*(B.x - O.x);
}

// Static
glm::vec2 PhysicsBody::b2Vec2ToGlm(const b2Vec2& vec)
{
	return glm::vec2(vec.x, vec.y);
}

// Static
b2Vec2 PhysicsBody::glmToB2Vec2(const glm::vec2& vec)
{
	return b2Vec2(vec.x, vec.y);
}




// Public
void PhysicsBody::calculateShape()
{
	calculateShape(mCircularShape);
}

// To have a non circular shape, you need to have an object geometry!
bool PhysicsBody::calculateShape(bool circularShape)
{
	if(mObjectGeometry)
	{
		mCircularShape = circularShape;
		mShape = createShapeFromObjectGeometry(*mObjectGeometry, circularShape);
		mRadius = mShape->m_radius;
	} else
	{
		if(circularShape)
		{
			mShape = createShapeFromRadius(mRadius); // Recalculates, maybe you would want this?
		} else
		{
			Utils::CRASH("Object geometry not defined! Cannot create non-circular shape!");
			return false;
		}
	}

	return true;
}

// Calculate circular shape
// circularShape bool still here to have a clean interface
bool PhysicsBody::calculateShape(bool circularShape, float radius)
{
	if(circularShape)
	{
		mShape = createShapeFromRadius(radius); // Recalculates, maybe you would want this?
		mRadius = radius;
	}
	else
	{
		Utils::CRASH("Cannot create non-circular shape with a radius!");
		return false;
	}

	return true;
}

bool PhysicsBody::isCircularShape()
{
	return mCircularShape;
}

float PhysicsBody::getRadius()
{
	return mRadius;
}

bool PhysicsBody::addToWorld(b2World& world, glm::vec2 position, float density)
{
	b2BodyDef bodyDef;
	bodyDef.position = glmToB2Vec2(position);

	mBody = world.CreateBody(&bodyDef);

	switch(mType)
	{
	case PHYSICS_BODY_IGNORED:
		break;

	case PHYSICS_BODY_STATIC:
		mBody->CreateFixture(mShape.get(), 0.0f); // Give the actual pointer since it will copy!
		break;

	case PHYSICS_BODY_DYNAMIC:
		mBody->CreateFixture(mShape.get(), density); // Funciton will copy the shape
		break;

	default:
		// Never should go here!
		Utils::CRASH("No valid physics body type specified! Did a check fail?");
		return false;
	}

	return true;
}