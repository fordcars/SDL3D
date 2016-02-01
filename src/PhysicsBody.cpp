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

// This class is where I started getting to close to my deadline

// All coords and everything are in meters from the physics engine's perspective!!!
// The only coords that are not in meters are the model's coords
// Can only be in one world at a time!

// Currently only supports top-down physics
// This means that Box2D's coords are only x and y (which is top down)
// So, Box2D's x is x in the 3D world, and Box2D's y is z in the 3D world

#include <PhysicsBody.hpp>

#include <Utils.hpp>
#include <GPUBuffer.hpp>
#include <Camera.hpp>
#include <ShadedObject.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cstddef> // For std::size_t
#include <algorithm>

#include <math.h> // For trig stuff

PhysicsBody::PhysicsBody()
{
	init();

	if(mType != PHYSICS_BODY_IGNORED)
		calculateShapes();
}

PhysicsBody::PhysicsBody(float radius, int type)
{
	init();

	mType = type;

	if(mType != PHYSICS_BODY_IGNORED)
		calculateShapes(radius);
}

PhysicsBody::PhysicsBody(constObjectGeometryPointer objectGeometry, bool circularShape, int type)
{
	init();

	mObjectGeometry = objectGeometry;
	mCircularShape = circularShape;
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
		calculateShapes();
}

// Will copy, but will not touch any world
PhysicsBody::PhysicsBody(const PhysicsBody& other)
{
	init();

	mObjectGeometry = other.mObjectGeometry;

	mPosition = glm::vec3(0.0f); // Don't copy the position, that would be weird
	mRotation = glm::vec3(0.0f);
	mDensity = other.mDensity;
	mFriction = other.mFriction;
	mRestitution = other.mRestitution;

	mScaling = other.mScaling;
	mCircularShape = other.mCircularShape;
	mRadius = other.mRadius;
	mType = other.mType;

	mPixelsPerMeter = PHYSICS_BODY_PIXELS_PER_METER;

	b2BlockAllocator block;

	if(mCircularShape)
		block.Allocate(sizeof(b2CircleShape));
	else
		block.Allocate(sizeof(b2ChainShape));

	mShapes.resize(other.mShapes.size());
	for(std::size_t i = 0; i < other.mShapes.size(); i++)
		mShapes[i] = shapeUniquePointer(other.mShapes[i]->Clone(&block));
}

PhysicsBody::~PhysicsBody()
{
	removeFromWorld();
}

void PhysicsBody::init()
{
	mWorldBody = nullptr;
	mWorld = nullptr;

	mPosition = glm::vec3(0.0f);
	mRotation = glm::vec3(0.0f);
	mDensity = 1.0f;
	mFriction = 1.0f;
	mRestitution = 0.1f;

	mScaling = glm::vec3(1.0f);
	mCircularShape = true;
	mRadius = 0.0f;
	mType = PHYSICS_BODY_IGNORED;

	// Number of pixels in the 3D object that would equal to a meter. Box2D likes meters, not pixels!
	mPixelsPerMeter = PHYSICS_BODY_PIXELS_PER_METER;
}

// Static
// Gives ownership
// Circular shapes have 1 shape
PhysicsBody::shapeVector PhysicsBody::createShapesFromObjectGeometry(const ObjectGeometry& objectGeometry,
	bool generateCircular, int pixelsPerMeter, glm::vec3 scaling)
{
	// b2Vec2 is a float32 point/vector
	std::vector<b2Vec2> positions2D = get2DCoordsObjectGeometry(objectGeometry, pixelsPerMeter, scaling);

	if(generateCircular)
	{
		shapeVector shapes;

		b2Vec2 centroid = getCentroid(positions2D);
		float radius = getCircleRadiusFromPoints(positions2D, centroid);
		
		b2CircleShape* circleShape = new b2CircleShape();
		circleShape->m_p = centroid;
		circleShape->m_radius = radius;

		// Moves the unique ptr!
		shapes.push_back(shapeUniquePointer(circleShape)); // Circular shapes have 1 shape

		return shapes;
	} else
	{
		shapeVector shapes;

		std::vector<p2t::Point> points = monotoneChainConvexHull(positions2D);

		// Here we must split the hull into smaller polygons (triangles) since Box2D has
		// a (low) max vertices per shape limit
		// Poly2tri takes pointers, dunno why
		std::vector<p2t::Point*> pointPointers = pointerizeP2t(points); // We delete these later in this function

		p2t::CDT cdt(pointPointers);
		cdt.Triangulate();
		std::vector<p2t::Triangle*> triangles = cdt.GetTriangles();

		shapes.resize(triangles.size());

		// Create shapes
		for(std::size_t i = 0; i < triangles.size(); i++)
		{
			p2t::Triangle* triangle = triangles[i];

			b2PolygonShape* newShape = new b2PolygonShape();

			// Convert to b2Vec2
			B2Vec2Vector trianglePoints{
				p2tToB2Vec2( *( triangle->GetPoint(0) ) ),
				p2tToB2Vec2( *( triangle->GetPoint(1) ) ),
				p2tToB2Vec2( *( triangle->GetPoint(2) ) )};

			newShape->Set(trianglePoints.data(), trianglePoints.size());

			shapes[i] = shapeUniquePointer(newShape);
		}

		// Delete all points
		for(std::size_t i = 0; i < pointPointers.size(); i++)
			delete pointPointers[i];

		return shapes;
	}
}

// Radius in meters
PhysicsBody::shapeUniquePointer PhysicsBody::createShapeFromRadius(float radius)
{
	b2CircleShape* circleShape = new b2CircleShape();
	circleShape->m_p = b2Vec2(0.0f, 0.0f);
	circleShape->m_radius = radius;

	return shapeUniquePointer(circleShape); // Moves the unique ptr!
}

// Static
// Heavy!
// Copies the data into a new vector
// Returns a b2Vec2Vector to assure compability with Box2D
// Returning coords are in meters!
PhysicsBody::B2Vec2Vector PhysicsBody::get2DCoordsObjectGeometry(const ObjectGeometry& objectGeometry,
	int pixelsPerMeter, glm::vec3 scaling)
{
	ObjectGeometry::uintVector indices = objectGeometry.getIndexBuffer().readData();
	ObjectGeometry::vec3Vector positions3D = objectGeometry.getPositionBuffer().readData();

	B2Vec2Vector positions2D(indices.size()); // Same number of elements

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
// Uses p2t::Points for compability with poly2tri
std::vector<p2t::Point> PhysicsBody::monotoneChainConvexHull(B2Vec2Vector points2D)
{
	int numberOfPoints = points2D.size(), hullIndex = 0; // THIS NEEDS TO BE AN INT (we need negative values!)
	std::vector<p2t::Point> hull(2 * numberOfPoints); // *2 to make sure

	// Sort points
	// Give a lamda since b2Vec2 don't define the >operator.
	std::sort(points2D.begin(), points2D.end(), [](const b2Vec2& first, const b2Vec2& second) // True if first should be before second
	{
		// Only compares the x-coords in this case. If the x-coords are the same, take the one with the smallest y-coord.
		return first.x < second.x || (first.x == second.x && first.y < second.y);
	});

	// Build lower hull
	for(int i = 0; i < numberOfPoints; i++)
	{
		p2t::Point point = B2Vec2ToP2t(points2D[i]);
		while(hullIndex >= 2 && cross(hull[hullIndex - 2], hull[hullIndex - 1], point) <= 0)
			hullIndex--;
		hull[hullIndex++] = point;
	}

	// Build upper hull
	for(int i = numberOfPoints-2, t = hullIndex+1; i >= 0; i--)
	{
		p2t::Point point = B2Vec2ToP2t(points2D[i]);
		while(hullIndex >= t && cross(hull[hullIndex - 2], hull[hullIndex - 1], point) <= 0)
			hullIndex--;
		hull[hullIndex++] = point;
	}
	
	hull.resize(hullIndex);

	// Remove similar points (Box2D loves this)
	for(std::size_t i = 0, length = hull.size(); i<100000; i++) // i++ since we will never remove the current element
	{
		if(i < length)
		{
			p2t::Point comparingPoint1 = hull[i];

			for(std::size_t k = 0; k<10000;)
			{
				if(k < length)
				{
					p2t::Point comparingPoint2 = hull[k];

					if(k != i) // Don't compare with the same point
					{
						if(distanceSquared(comparingPoint1, comparingPoint2) <= 0.005f * 0.005f)
						{
							hull.erase(hull.begin() + k); // Erase one of them
							length--;
						} else
						{
							k++;
						}
					} else
					{
						k++;
					}
				} else
				{
					break;
				}
			}
		} else
		{
			break;
		}
	}

	return hull; // Note: C++ automatically moves values instead of copying when returning
}

// Static
// From http://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon
b2Vec2 PhysicsBody::getCentroid(const B2Vec2Vector& points2D)
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
float PhysicsBody::getCircleRadiusFromPoints(B2Vec2Vector points2D, b2Vec2 centroid)
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
// Copies and creates a bunch f pointers, you must free them after!
std::vector<p2t::Point*> PhysicsBody::pointerizeP2t(const std::vector<p2t::Point>& points)
{
	std::size_t pointCount = points.size();
	std::vector<p2t::Point*> pointers(pointCount);

	for(std::size_t i = 0; i < pointCount; i++)
		pointers[i] = new p2t::Point(points[i]);

	return pointers;
}

// Static
float PhysicsBody::cross(const p2t::Point& O, const p2t::Point& A, const p2t::Point& B)
{
	return static_cast<float>((A.x - O.x)*(B.y - O.y) - (A.y - O.y)*(B.x - O.x));
}

// Static
float PhysicsBody::distanceSquared(const p2t::Point& A, const p2t::Point& B)
{
	return static_cast<float>((B.x - A.x)*(B.x - A.x) + (B.y - A.y)*(B.y - A.y));
}

// Static
// A generic function for generating circle vertices
// Angle in degrees
PhysicsBody::vec2Vector PhysicsBody::getCircleVertices(glm::vec2 origin, float radius, int angleIncrementation)
{
	vec2Vector vertices;

	float radianIncrementation = degreesToRadians(static_cast<float>(angleIncrementation));
	int incrementationCount = 360 / angleIncrementation;

	float currentRadians = 0.0f;

	for(int i = 0; i < incrementationCount; i++)
	{
		currentRadians += radianIncrementation;

		float x = origin.x + (radius * cos(currentRadians));
		float y = origin.y + (radius * sin(currentRadians));

		vertices.push_back(glm::vec2(x, y));
	}

	return vertices;
}

// bodyDef IS modified!
PhysicsBody::fixtureDefVector PhysicsBody::generateFixtureDefsAndSetBodyDef(b2BodyDef& bodyDef)
{
	fixtureDefVector fixtureDefs;

	if(mShapes.empty())
	{
		Utils::CRASH("No shapes calculated for this physics body! Cannot generate fixture definitions!");
		return fixtureDefVector(0);
	}

	for(auto &shape : mShapes)
	{
		b2FixtureDef fixtureDef;

		fixtureDef.density = mDensity;
		fixtureDef.friction = mFriction;
		fixtureDef.restitution = mRestitution;

		switch(mType)
		{
		case PHYSICS_BODY_IGNORED:
			break;

		case PHYSICS_BODY_STATIC:
			bodyDef.type = b2_staticBody;
			break;

		case PHYSICS_BODY_KINEMATIC:
			bodyDef.type = b2_kinematicBody;
			break;

		case PHYSICS_BODY_DYNAMIC:
		{ // Brakets for a new scope, lets us declar variables
			bodyDef.type = b2_dynamicBody;
			break;
		}

		default:
			// Never should go here!
			Utils::CRASH("No valid physics body type specified! Did a check fail?");
			break;
		}

		fixtureDef.shape = shape.get();
		fixtureDefs.push_back(fixtureDef);
	}

	return fixtureDefs;
}

// Call when you want to push new shapes (fixtures) to the body
bool PhysicsBody::updateWorldBodyFixtures()
{
	if(!mShapes.empty())
	{
		if(mWorldBody)
		{
			// Remove old fixtures
			// http://box2d.org/forum/viewtopic.php?f=3&t=8414
			for(b2Fixture* oldFixture = mWorldBody->GetFixtureList(); oldFixture;)
			{
				// We need to destroy a copy of the pointer to destroy the fixture and not our reference (oldFixture)!
				b2Fixture* fixtureToDestroy = oldFixture;
				oldFixture = oldFixture->GetNext();

				mWorldBody->DestroyFixture(fixtureToDestroy);
			}

			b2BodyDef dummyDef;
			// This function takes a body def, but we want our current one instead
			// This function takes a body def since I wanted to limit the amount of functions that checked the physics body type
			fixtureDefVector newFixtureDefs = generateFixtureDefsAndSetBodyDef(dummyDef);

			for(auto &fixtureDef : newFixtureDefs)
				mWorldBody->CreateFixture(&fixtureDef);

			return true;
		}
		else
		{
			Utils::CRASH("This physics body was not added to a world! Cannot update body fixtures! Please add this body to the world before calling.");
			return false;
		}
	}
	else
	{
		Utils::CRASH("No shapes calculated for body! Cannot update body fixtures! Please calculate the shape before updating.");
		return false;
	}
}

// Public
// Will (re)calculate the shape, circular or not
bool PhysicsBody::calculateShapes()
{
	if(mCircularShape && !mObjectGeometry) // If it is a simple circle, no object geometry
		return calculateShapes(mRadius);
	else
		return calculateShapes(mCircularShape, mScaling);
}

// To have a non circular shape, you need to have an object geometry!
bool PhysicsBody::calculateShapes(bool circularShape, glm::vec3 scaling)
{
	if(mObjectGeometry)
	{
		mCircularShape = circularShape;
		mShapes = createShapesFromObjectGeometry(*mObjectGeometry, circularShape, mPixelsPerMeter, scaling);
		mScaling = scaling;

		if(circularShape)
			mRadius = mShapes[0]->m_radius; // A circular shape is 1 shape

		if(mWorldBody)
			updateWorldBodyFixtures();
	} else
	{
		Utils::CRASH("Object geometry not defined! Cannot create shape from object geometry!");
		return false;
	}

	return true;
}

// Calculate circular shape
bool PhysicsBody::calculateShapes(float radius)
{
	// A circle shape is 1 shape
	// Make sure we don't do something weird, make sure our new shape is the only one
	// (so don't use push_back and friends)
	mShapes.resize(1);
	mShapes[0] = createShapeFromRadius(radius);
	mRadius = radius;

	if(mWorldBody)
		updateWorldBodyFixtures();

	return true;
}

void PhysicsBody::setDensity(float density)
{
	mDensity = density; // Whatever happens, we want future bodies to have this density

	if(mWorldBody)
	{
		for(b2Fixture* fixture = mWorldBody->GetFixtureList(); fixture; fixture = fixture->GetNext())
			fixture->SetDensity(density);
	} // Who cares if there is no body, when we will create it it will have the right density
}

float PhysicsBody::getDensity() const
{
	return mDensity;
}

void PhysicsBody::setFriction(float friction)
{
	mFriction = friction; // Whatever happens, we want future bodies to have this density

	if(mWorldBody)
	{
		for(b2Fixture* fixture = mWorldBody->GetFixtureList(); fixture; fixture = fixture->GetNext())
			fixture->SetFriction(friction);
	}
}

float PhysicsBody::getFriction() const
{
	return mFriction;
}

// Bounciness, normally set between 0 and 1 (1 = bounces at the intensity that it hit)
// Seems to only work when there are forces involved, not just velocities
void PhysicsBody::setRestitution(float restitution)
{
	mRestitution = restitution; // Whatever happens, we want future bodies to have this density

	if(mWorldBody)
	{
		for(b2Fixture* fixture = mWorldBody->GetFixtureList(); fixture; fixture = fixture->GetNext())
			fixture->SetRestitution(restitution);
	}
}

float PhysicsBody::getRestitution() const
{
	return mRestitution;
}

int PhysicsBody::getPixelsPerMeter() const
{
	return mPixelsPerMeter;
}

bool PhysicsBody::isCircularShape() const
{
	return mCircularShape;
}

float PhysicsBody::getRadius() const
{
	return mRadius;
}

int PhysicsBody::getType() const
{
	return mType;
}

// In meters, as always
void PhysicsBody::setPosition(glm::vec3 position)
{
	mPosition = position;

	if(mWorldBody)
	{
		b2Vec2 pos2D = b2Vec2(position.x, position.z);
		mWorldBody->SetTransform(pos2D, mWorldBody->GetAngle());
	}
}

// Returns the real position if it is in a world, or the position it would of have been in a world
glm::vec3 PhysicsBody::getPosition() const
{
	if(mWorldBody)
	{
		glm::vec2 pos2D = B2Vec2ToGlm(mWorldBody->GetPosition());
		return glm::vec3(pos2D.x, mPosition.y, pos2D.y);
	}

	return mPosition;
}

// In degrees
void PhysicsBody::setRotation(glm::vec3 angle)
{
	mRotation = angle;

	if(mWorldBody)
	{
		// The Box2D angle is equivalent to our y rotation
		mWorldBody->SetTransform(mWorldBody->GetPosition(), degreesToRadians(angle.y));
	}
}

// In degrees
glm::vec3 PhysicsBody::getRotation() const
{
	if(mWorldBody)
	{
		float yRotation = radiansToDegrees(mWorldBody->GetAngle()); // The Box2D angle is equivalent to our y rotation
		return glm::vec3(mRotation.x, yRotation, mRotation.z);
	}

	return mRotation;
}

glm::vec3 PhysicsBody::getRotationInRadians() const
{
	if(mWorldBody)
	{
		float yRotation = mWorldBody->GetAngle();

		return glm::vec3(
			degreesToRadians(mRotation.x),
			yRotation,
			degreesToRadians(mRotation.z));
	}
	
	return glm::vec3(
		degreesToRadians(mRotation.x),
		degreesToRadians(mRotation.y),
		degreesToRadians(mRotation.z));
}

// Takes a vec3 to be coherent with the rest
// The non-physics coord is useless
void PhysicsBody::setVelocity(glm::vec3 velocity)
{
	mVelocity = velocity;

	if(mWorldBody)
	{
		b2Vec2 velocity2D = b2Vec2(velocity.x, velocity.z);
		mWorldBody->SetLinearVelocity(velocity2D);
	}
}

glm::vec3 PhysicsBody::getVelocity() const
{
	if(mWorldBody)
	{
		b2Vec2 velocity2D = mWorldBody->GetLinearVelocity();
		return glm::vec3(velocity2D.x, mVelocity.y, velocity2D.y);
	}

	return mVelocity;
}

// False on error
// The world needs to stay alive while this body exists
bool PhysicsBody::addToWorld(b2World* world)
{
	b2BodyDef bodyDef;

	if(mType == PHYSICS_BODY_IGNORED)
		return true;

	if(!mShapes.empty())
	{
		if(!mWorld) // Not already added!
		{
			mWorld = world;

			bodyDef.position = b2Vec2(mPosition.x, mPosition.z);
			bodyDef.angle = mRotation.y;
			bodyDef.linearVelocity = b2Vec2(mVelocity.x, mVelocity.z);
			fixtureDefVector fixtureDefs = generateFixtureDefsAndSetBodyDef(bodyDef);

			mWorldBody = world->CreateBody(&bodyDef); // Call this after generating fixture defs!

			for(auto &fixtureDef : fixtureDefs)
				mWorldBody->CreateFixture(&fixtureDef);

			return true;
		} else
		{
			Utils::CRASH("This physics body was already added to a world! Cannot add again.");
			return false;
		}

	} else
	{
		Utils::CRASH("No shapes calculated for physics body! Cannot add to world! Please calculate the shapes before adding it to the world.");
		return false;
	}
}

void PhysicsBody::removeFromWorld()
{
	if(mWorld)
		mWorld->DestroyBody(mWorldBody);

	mWorldBody = nullptr;
	mWorld = nullptr;
}

// Not including scaling can be useful
// For example, rendering coords that have already been scaled (like the physics shape)
glm::mat4 PhysicsBody::generateModelMatrix(bool includeScaling)
{
	// Scaling * rotation * translation
	glm::vec3 calculatedRotation = getRotationInRadians();

	glm::mat4 translationM = glm::translate(glm::mat4(1.0f), getPosition() * static_cast<float>(mPixelsPerMeter));

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

	glm::mat4 modelM;
	
	if(includeScaling)
		modelM = glm::scale(rotationXYZM, mScaling);
	else
		modelM = rotationXYZM;

	return modelM;
}

// A quick an easy renderer for debugging
// Slow!
// Other 3D coord is the non-physics coord that the physics body will be drawn at
// Takes a shader pointer to be consistent
void PhysicsBody::renderDebugShape(constShaderPointer shader, const Camera* camera, float other3DCoord)
{
	if(mShapes.empty())
	{
		Utils::CRASH("Cannot debug render this physics body, it does not have shapes! Please calculate them before calling.");
		return;
	}

	// Lets do something very smart, create a buffer, fill it, and then delete it each time we render
	GPUBuffer<glm::vec3> positionBuffer;
	vec3Vector positions3D;

	if(mCircularShape)
	{
		// One shape per circular shape
		b2CircleShape* circle = static_cast<b2CircleShape*>(mShapes[0].get());

		// Translation is done in the model matrix
		vec2Vector vertices2D = getCircleVertices(glm::vec2(0.0f), circle->m_radius, 10);

		for(auto &vertex2D : vertices2D)
		{
			positions3D.push_back(glm::vec3(
				vertex2D.x * mPixelsPerMeter,
				other3DCoord * mPixelsPerMeter,
				vertex2D.y * mPixelsPerMeter));
		}

		// Add line to see circle angle better

		// Front is the first element
		// We need to add this otherwise we see a hole in the circle
		positions3D.push_back(positions3D.front());
		positions3D.push_back(glm::vec3(0.0f)); // Add the center of the circle

		positionBuffer.setMutableData(positions3D, GL_STATIC_DRAW);
	} else
	{
		for(std::size_t i = 0; i < mShapes.size(); i++)
		{
			b2Shape* shape = mShapes[i].get();
			// We can static cast here, since we know 100% it is a polygon shape.
			b2PolygonShape* polygon = static_cast<b2PolygonShape*>(shape);

			// Go through each points of the triangle shape
			for(int pointIndex = 0; pointIndex < polygon->GetVertexCount(); pointIndex++)
			{
				// Add each point to the buffer
				b2Vec2 point2D = polygon->GetVertex(pointIndex);
				positions3D.push_back(glm::vec3(
					point2D.x * mPixelsPerMeter,
					other3DCoord * mPixelsPerMeter,
					point2D.y * mPixelsPerMeter));
			}
		}
		
		positionBuffer.setMutableData(positions3D, GL_STATIC_DRAW);
	}

	glm::mat4 modelMatrix = generateModelMatrix(false);
	glm::mat4 MVP = camera->getProjectionMatrix() * camera->getViewMatrix() * modelMatrix;

	glUseProgram(shader->getID());
	glUniformMatrix4fv(shader->findUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

	glEnableVertexAttribArray(0); // Number to give to OpenGL VertexAttribPointer
	positionBuffer.bind(GL_ARRAY_BUFFER);

	// Give it to the shader. Each time the vertex shader runs, it will get the next element of this buffer.
	glVertexAttribPointer(
		0,					// Attribute 0, no particular reason but same as the vertex shader's layout and glEnableVertexAttribArray
		3,					// Size. Number of values per vertex, must be 1, 2, 3 or 4.
		GL_FLOAT,			// Type of data (GLfloats)
		GL_FALSE,			// Normalized?
		0,					// Stride
		(void*)0			// Array buffer offset
		);

	// Draw!
	glDrawArrays(
		GL_LINE_STRIP, // Mode
		0, // First
		positions3D.size() // Count
		);

	glDisableVertexAttribArray(0);
}