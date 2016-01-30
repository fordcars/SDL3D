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

// All coords and everything are in meters from the physics engine's perspective!!!
// The only coords that are not in meters are the model's coords

// Currently only supports top-down physics
// This means that Box2D's coords are only x and y (which is top down)
// So, Box2D's x is x in the 3D world, and Box2D's y is z in the 3D world

#include <PhysicsBody.hpp>

#include <Utils.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cstddef> // For std::size_t
#include <algorithm>

PhysicsBody::PhysicsBody()
{
	mWorldBody = nullptr;
	mWorld = nullptr;
	mObjectGeometry = nullptr;

	mPosition = glm::vec3(0.0f);
	mRotation = glm::vec3(0.0f);
	mDensity = 0.0f;
	mFriction = 0.0f;

	mScaling = glm::vec3(1.0f);
	mCircularShape = true;
	mRadius = 0.0f;
	mType = PHYSICS_BODY_STATIC;

	// Number of pixels in the 3D object that would equal to a meter. Box2D lieks meters, not pixels!
	mPixelsPerMeter = PHYSICS_BODY_PIXELS_PER_METER;
}

PhysicsBody::PhysicsBody(ObjectGeometry* objectGeometry, bool circularShape, int type)
{
	mWorldBody = nullptr; // nullptr is a keyword
	mWorld = nullptr;
	mObjectGeometry = nullptr;

	mPosition = glm::vec3(0.0f);
	mRotation = glm::vec3(0.0f);
	mDensity = 0.0f;
	mFriction = 0.0f;

	mScaling = glm::vec3(1.0f);
	mCircularShape = circularShape;
	mRadius = 0.0f;
	mType = type;

	// Number of pixels in the 3D object that would equal to a meter. Box2D lieks meters, not pixels!
	mPixelsPerMeter = PHYSICS_BODY_PIXELS_PER_METER;

	if(type != PHYSICS_BODY_IGNORED &&
		type != PHYSICS_BODY_STATIC &&
		type != PHYSICS_BODY_KINEMATIC &&
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
	mWorldBody = nullptr;
	mWorld = nullptr;
	mObjectGeometry = other.mObjectGeometry;

	mPosition = glm::vec3(0.0f); // Don't copy the position, that would be weird
	mRotation = glm::vec3(0.0f);
	mDensity = other.mDensity;
	mFriction = other.mFriction;

	mScaling = glm::vec3(1.0f);
	mCircularShape = other.mCircularShape;
	mRadius = other.mRadius;
	mType = other.mType;

	mPixelsPerMeter = PHYSICS_BODY_PIXELS_PER_METER;

	b2BlockAllocator block;

	if(mCircularShape)
		block.Allocate(sizeof(b2CircleShape));
	else
		block.Allocate(sizeof(b2PolygonShape));

	mShape = b2ShapeUniquePointer(other.mShape->Clone(&block));
}

PhysicsBody::~PhysicsBody()
{
	if(mWorld)
	{
		mWorld->DestroyBody(mWorldBody);
	} // Who cares if it wasn't added to a world, less cleaning to do on our side
}

// Static
// Gives ownership
PhysicsBody::b2ShapeUniquePointer PhysicsBody::createShapeFromObjectGeometry(const ObjectGeometry& objectGeometry,
	bool generateCircular, int pixelsPerMeter, glm::vec3 scaling)
{
	// b2Vec2 is a float32 point/vector
	std::vector<b2Vec2> positions2D = get2DCoordsObjectGeometry(objectGeometry, pixelsPerMeter, scaling);

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

// Radius in meters
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
// Returns a b2Vec2 to assure compability with Box2D
PhysicsBody::b2Vec2Vector PhysicsBody::get2DCoordsObjectGeometry(const ObjectGeometry& objectGeometry,
	int pixelsPerMeter, glm::vec3 scaling)
{
	ObjectGeometry::uintVector indices = objectGeometry.getIndexBuffer().readData();
	ObjectGeometry::vec3Vector positions3D = objectGeometry.getPositionBuffer().readData();

	b2Vec2Vector positions2D(indices.size()); // Same number of elements

	// Convert polygons to 2D
	for(std::size_t i = 0; i < indices.size(); i++)
	{
		unsigned int vertexIndex = indices[i]; // The index in the positions vector of this vertex
		glm::vec3 vertexPosition3D = positions3D[vertexIndex];

		positions2D[i] = b2Vec2((vertexPosition3D.x * scaling.x) / pixelsPerMeter,
			(vertexPosition3D.z * scaling.z) / pixelsPerMeter);
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

bool PhysicsBody::updateBodyFixture()
{
	if(mShape)
	{
		if(mWorldBody)
		{
			b2Fixture *currentFixture = &mWorldBody->GetFixtureList()[0]; // For us, one fixture per body!
			mWorldBody->DestroyFixture(currentFixture);

			b2BodyDef dummyDef;
			// This function takes a body def, but we want our current one instead
			// This function takes a body def since I wanted to limit the amount of functions that checked the physics body type
			b2FixtureDef newFixture = generateFixtureDefAndSetBodyDef(dummyDef);

			mWorldBody->CreateFixture(&newFixture);

			return true;
		} else
		{
			Utils::CRASH("This physics body was not added to a world! Cannot update it! Please add this body to the world before calling.");
			return false;
		}
	} else
	{
		Utils::CRASH("No shape calculated for body! Cannot update the body! Please calculate the shape before updating.");
		return false;
	}
}

// bodyDef IS modified!
b2FixtureDef PhysicsBody::generateFixtureDefAndSetBodyDef(b2BodyDef& bodyDef)
{
	b2FixtureDef fixtureDef;

	switch(mType)
	{
	case PHYSICS_BODY_IGNORED:
		break;

	case PHYSICS_BODY_STATIC:
		bodyDef.type = b2_staticBody;
		break;

	case PHYSICS_BODY_KINEMATIC:
		bodyDef.type = b2_kinematicBody;
		fixtureDef.density = mDensity;
		break;

	case PHYSICS_BODY_DYNAMIC:
	{ // Brakets for a new scope, lets us declar variables
		bodyDef.type = b2_dynamicBody;
		fixtureDef.density = mDensity;
		fixtureDef.friction = mFriction;
		break;
	}

	default:
		// Never should go here!
		Utils::CRASH("No valid physics body type specified! Did a check fail?");
		break;
	}

	if(mShape)
		fixtureDef.shape = mShape.get();
	else
		Utils::CRASH("No shape calculated for this physics body! Cannot generate fixture definition!");

	return fixtureDef;
}

// Public
// Will (re)calculate the shape, circular or not
bool PhysicsBody::calculateShape()
{
	return calculateShape(mCircularShape, mScaling);
}

// To have a non circular shape, you need to have an object geometry!
bool PhysicsBody::calculateShape(bool circularShape, glm::vec3 scaling)
{
	if(mObjectGeometry)
	{
		mCircularShape = circularShape;
		mShape = createShapeFromObjectGeometry(*mObjectGeometry, circularShape, mPixelsPerMeter, scaling);
		mRadius = mShape->m_radius;
	} else
	{
		Utils::CRASH("Object geometry not defined! Cannot create non-circular shape!");
		return false;
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

void PhysicsBody::setDensity(float density)
{
	mDensity = density; // Whatever happens, we want future bodies to have this density

	if(mWorldBody)
	{
		b2Fixture *fixture = &mWorldBody->GetFixtureList()[0];
		fixture->SetDensity(density);
	} // Who cares if there is no body, when we will create it it will have the right density
}

float PhysicsBody::getDensity()
{
	return mDensity;
}

void PhysicsBody::setFriction(float friction)
{
	mFriction = friction; // Whatever happens, we want future bodies to have this density

	if(mWorldBody)
	{
		b2Fixture *fixture = &mWorldBody->GetFixtureList()[0];
		fixture->SetFriction(friction);
	}
}

float PhysicsBody::getFriction()
{
	return mFriction;
}

bool PhysicsBody::isCircularShape()
{
	return mCircularShape;
}

float PhysicsBody::getRadius()
{
	return mRadius;
}

bool PhysicsBody::setType(int type)
{
	mType = type;
	setPosition
	return updateBodyFixture();
}

int PhysicsBody::getType()
{
	return mType;
}

bool PhysicsBody::setPosition(glm::vec3 position)
{
	if(mType == PHYSICS_BODY_IGNORED)
	{
		mPosition = position;
		return true;
	}

	if(mWorldBody)
	{
		b2Vec2 pos2D = b2Vec2(position.x, position.z);

		mPosition = position;
		mWorldBody->SetTransform(pos2D, mWorldBody->GetAngle());
		return true;
	}
	else
	{
		Utils::CRASH("This physics body was not added to a world! Cannot set position. Please add it to a world first.");
		return false;
	}
}

glm::vec3 PhysicsBody::getPosition()
{
	if(mType == PHYSICS_BODY_IGNORED)
	{
		return mPosition;
	}

	if(mWorldBody)
	{
		glm::vec2 pos2D = b2Vec2ToGlm(mWorldBody->GetPosition());
		mPosition = glm::vec3(pos2D.x, mPosition.y, pos2D.y);

		return mPosition;
	} else
	{
		Utils::CRASH("This physics body was not added to a world! Cannot get position. Please add it to a world first.");
		return glm::vec3(0.0f);
	}
}

// In degrees
bool PhysicsBody::setRotation(glm::vec3 angle)
{
	if(mType == PHYSICS_BODY_IGNORED)
	{
		mRotation = angle;
		return true;
	}

	if(mWorldBody)
	{
		mRotation = angle;
		// The Box2D angle is equivalent to our y rotation
		mWorldBody->SetTransform(mWorldBody->GetPosition(), degreesToRadians(angle.y));
		return true;
	}
	else
	{
		Utils::CRASH("This physics body was not added to a world! Cannot set rotation. Please add it to a world first.");
		return false;
	}
}

// In degrees
glm::vec3 PhysicsBody::getRotation()
{
	if(mType == PHYSICS_BODY_IGNORED)
		return mRotation;

	if(mWorldBody)
	{
		float yRotation = radiansToDegrees(mWorldBody->GetAngle()); // The Box2D angle is equivalent to our y rotation
		mRotation = glm::vec3(mRotation.x, yRotation, mRotation.z);

		return mRotation;
	}
	else
	{
		Utils::CRASH("This physics body was not added to a world! Cannot get rotation. Please add it to a world first.");
		return glm::vec3(0.0f);
	}
}

glm::vec3 PhysicsBody::getRotationInRadians()
{
	if(mType == PHYSICS_BODY_IGNORED)
		return glm::vec3(
			degreesToRadians(mRotation.x),
			degreesToRadians(mRotation.y),
			degreesToRadians(mRotation.z));

	if(mWorldBody)
	{
		float yRotation = mWorldBody->GetAngle();

		mRotation = getRotation(); // Keep track of rotation in degrees
		return glm::vec3(
			degreesToRadians(mRotation.x),
			yRotation,
			degreesToRadians(mRotation.z));
	}
	else
	{
		Utils::CRASH("This physics body was not added to a world! Cannot get rotation in radians (or degrees!).Please add it to a world first.");
		return glm::vec3(0.0f);
	}
}

bool PhysicsBody::setVelocity(glm::vec3 velocity) // Takes a vec3 to be coherent with the rest
{
	if(mWorldBody)
	{
		b2Vec2 velocity2D = b2Vec2(velocity.x, velocity.z);
		mWorldBody->SetLinearVelocity(velocity2D);
		return true;
	}
	else
	{
		Utils::CRASH("This physics body was not added to a world! Cannot set velocity. Please add it to a world first.");
		return false;
	}
}

glm::vec3 PhysicsBody::getVelocity()
{
	if(mWorldBody)
	{
		b2Vec2 velocity2D = mWorldBody->GetLinearVelocity();
		return glm::vec3(velocity2D.x, 0.0f, velocity2D.y); // There is no velocity for the y-coords
	}
	else
	{
		Utils::CRASH("This physics body was not added to a world! Cannot get velocity. Please add it to a world first.");
		return glm::vec3(0.0f);
	}
}

bool PhysicsBody::addToWorld(b2World& world, glm::vec2 position, float density)
{
	b2BodyDef bodyDef;

	if(mShape)
	{
		if(!mWorld) // Not already added!
		{
			bodyDef.position = glmToB2Vec2(position);
			mWorldBody = world.CreateBody(&bodyDef);

			b2FixtureDef fixtureDef = generateFixtureDefAndSetBodyDef(bodyDef);
			mWorldBody->CreateFixture(&fixtureDef);

			return true;
		} else
		{
			Utils::CRASH("This physics body was already added to a world! Cannot add again.");
			return false;
		}

	} else
	{
		Utils::CRASH("No shape calculated for physics body! Cannot add to world! Please calculate the shape before adding it to the world.");
		return false;
	}
}

glm::mat4 PhysicsBody::generateModelMatrix()
{
	// Scaling * rotation * translation
	glm::vec3 calculatedRotation = getRotationInRadians();

	glm::mat4 translationM = glm::translate(glm::mat4(1.0f), getPosition());

	// Big chunk since we have to do x, y and z rotation manually
	glm::mat4 rotationXM = glm::rotate(translationM,
		calculatedRotation.x, // Glm takes radians
		glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 rotationXYM = glm::rotate(rotationXM,
		calculatedRotation.y,
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 rotationXYZM = glm::rotate(rotationXYM,
		calculatedRotation.z,
		glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 modelM = glm::scale(rotationXYZM, mScaling * static_cast<float>(mPixelsPerMeter));

	return modelM;
}