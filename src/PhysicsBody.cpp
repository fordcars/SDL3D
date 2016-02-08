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

// Keep in mind that Box2D rotations are the opposite of ours!! (* -1)
// (for Box2D, clockwise rotation = angle increases)

// Note: Box2D adds a skin around bodies to prevent tunnelling, so there might be a small visible gap between bodeis

// I would like to keep this class as close to 3D as possible (no 2D specific methods)

#include <PhysicsBody.hpp>

#include <Utils.hpp>
#include <GPUBuffer.hpp>
#include <Camera.hpp>
#include <ShadedObject.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cstddef> // For std::size_t
#include <algorithm>
#include <cfloat> // For angle checking

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
	mIsCircular = circularShape;
	mType = type;

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
	mVelocity = glm::vec3(0.0f);

	mIsBullet = other.mIsBullet;
	mIsFixtedRotation = other.mIsFixtedRotation;

	mDensity = other.mDensity;
	mFriction = other.mFriction;
	mRestitution = other.mRestitution;
	mWorldFriction = other.mWorldFriction;

	mScaling = other.mScaling;
	mIsCircular = other.mIsCircular;
	mRadius = other.mRadius;
	mType = other.mType;

	b2BlockAllocator block;

	if(mIsCircular)
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
	mVelocity = glm::vec3(0.0f);
	mIsBullet = false;
	mIsFixtedRotation = false;

	mDensity = 1.0f;
	mFriction = 1.0f;
	mRestitution = 0.1f;
	mWorldFriction = 1.0f;

	mScaling = glm::vec3(1.0f);
	mIsCircular = true;
	mRadius = 0.0f;
	mType = PHYSICS_BODY_IGNORED;
}

// Static
// Gives ownership
// Circular shapes have 1 shape
// Rotation and scaling are important here
PhysicsBody::shapeVector PhysicsBody::createShapesFromObjectGeometry(const ObjectGeometry& objectGeometry,
	bool generateCircular, float pixelsPerMeter, glm::vec3 rotation, glm::vec3 scaling)
{
	// b2Vec2 is a float32 point/vector
	std::vector<b2Vec2> positions2D = get2DObjectGeometryCoords(objectGeometry, pixelsPerMeter, rotation, scaling);

	if(generateCircular)
	{
		shapeVector shapes;

		b2Vec2 centroid = get2DCentroid(positions2D);
		float radius = getCircleRadiusFromPoints(positions2D, centroid);
		
		b2CircleShape* circleShape = new b2CircleShape();
		circleShape->m_p = centroid; // The local position (compared to the whole geometry)
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
PhysicsBody::shapeUniquePointer PhysicsBody::createShapesFromRadius(float radius)
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
PhysicsBody::B2Vec2Vector PhysicsBody::get2DObjectGeometryCoords(const ObjectGeometry& objectGeometry,
	float pixelsPerMeter, glm::vec3 rotation, glm::vec3 scaling)
{
	ObjectGeometry::uintVector indices = objectGeometry.getIndexBuffer().readData();
	ObjectGeometry::vec3Vector positions3D = objectGeometry.getPositionBuffer().readData();

	// generate a matrix so we can easily have rotation and scaling
	glm::mat4 matrix = generateModelMatrix(glm::vec3(0.0f), rotation, scaling / pixelsPerMeter);
	B2Vec2Vector positions2D(indices.size()); // Same number of elements

	// Convert polygons to 2D
	for(std::size_t i = 0; i < indices.size(); i++)
	{
		unsigned int vertexIndex = indices[i]; // The index in the positions vector of this vertex
		glm::vec3 vertexPosition3D = positions3D[vertexIndex];

		glm::vec4 transformedPoint = matrix * glm::vec4(vertexPosition3D, 1.0f); // 1 for point

		positions2D[i] = b2Vec2(transformedPoint.x, transformedPoint.z);
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

	// Remove similar points (Box2D loves this, poly2tri requires this)
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
b2Vec2 PhysicsBody::get2DCentroid(const B2Vec2Vector& points2D)
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

// Static
// Translation in meters and rotation in degrees as always
glm::mat4 PhysicsBody::generateModelMatrix(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scaling)
{
	glm::mat4 modelM;

	// Scaling * rotation * translation
	glm::vec3 rotationInRadians = glm::vec3(
		degreesToRadians(rotation.x),
		degreesToRadians(rotation.y),
		degreesToRadians(rotation.z));

	glm::mat4 translationM = glm::translate(glm::mat4(1.0f), translation * PHYSICS_PIXELS_PER_METER);

	// Big chunk since we have to do x, y and z rotation manually
	glm::mat4 rotationXM = glm::rotate(translationM,
		rotationInRadians.x, // Glm takes radians
		glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 rotationXYM = glm::rotate(rotationXM,
		rotationInRadians.y,
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 rotationXYZM = glm::rotate(rotationXYM,
		rotationInRadians.z,
		glm::vec3(0.0f, 0.0f, 1.0f));

	modelM = glm::scale(rotationXYZM, scaling);

	return modelM;
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

	bodyDef.position = b2Vec2(mPosition.x, mPosition.z);
	bodyDef.angle = mRotation.y;
	bodyDef.linearVelocity = b2Vec2(mVelocity.x, mVelocity.z);
	bodyDef.bullet = mIsBullet;
	bodyDef.fixedRotation = mIsFixtedRotation;

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
// Useful when the object's geometry changed
// Does not recalculate shape for speed (since you wouldn't want, for example, to
// recalculate for each animation frame or whatever).
void PhysicsBody::setObjectGeometry(constObjectGeometryPointer objectGeometry)
{
	mObjectGeometry = objectGeometry;
}

// Will (re)calculate the shape, circular or not
bool PhysicsBody::calculateShapes()
{
	if(mIsCircular && !mObjectGeometry) // If it is a simple circle, no object geometry
		return calculateShapes(mRadius);
	else
		return calculateShapes(mIsCircular, mScaling);
}

// To have a non circular shape, you need to have an object geometry!
bool PhysicsBody::calculateShapes(bool isCircularShape, glm::vec3 scaling)
{
	if(mObjectGeometry)
	{
		mIsCircular = isCircularShape;
		mShapes = createShapesFromObjectGeometry(*mObjectGeometry, isCircularShape, PHYSICS_PIXELS_PER_METER,
			mRotation, scaling);
		mScaling = scaling;

		if(isCircularShape)
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
	mShapes[0] = createShapesFromRadius(radius);
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

// Friction is a proportion of the mass
void PhysicsBody::setWorldFriction(float friction)
{
	mWorldFriction = friction;
}

float PhysicsBody::getWorldFriction() const
{
	return mWorldFriction;
}

bool PhysicsBody::isCircular() const
{
	return mIsCircular;
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
		mWorldBody->SetTransform(mWorldBody->GetPosition(), -degreesToRadians(angle.y)); // Box2D angle is reversed!
	}
}

void PhysicsBody::setRotationInRadians(glm::vec3 angle)
{
	mRotation = angle;

	if(mWorldBody)
	{
		mWorldBody->SetTransform(mWorldBody->GetPosition(), -angle.y); // Box2D angle is reversed!
	}
}

// In degrees
glm::vec3 PhysicsBody::getRotation() const
{
	if(mWorldBody)
	{
		// The Box2D angle is equivalent to our y rotation
		float yRotation = radiansToDegrees(-mWorldBody->GetAngle()); // Box2D angles are inversed!
		return glm::vec3(mRotation.x, yRotation, mRotation.z);
	}

	return mRotation;
}

glm::vec3 PhysicsBody::getRotationInRadians() const
{
	if(mWorldBody)
	{
		float yRotation = -mWorldBody->GetAngle();

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

// The non-physics coord still moves the body on the axis
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

void PhysicsBody::setBullet(bool isBullet)
{
	mIsBullet = isBullet;

	if(mWorldBody)
		mWorldBody->SetBullet(isBullet);
}

bool PhysicsBody::isBullet() const
{
	return mIsBullet;
}

void PhysicsBody::setFixtedRotation(bool fixted)
{
	mIsFixtedRotation = fixted;

	if(mWorldBody)
		mWorldBody->SetFixedRotation(fixted);
}

bool PhysicsBody::isFixtedRotation() const
{
	return mIsFixtedRotation;
}

// Gets the local 2D center of the shapes (relative to their location in the geometry)
// Not heavy
glm::vec2 PhysicsBody::getShapesLocal2DCenter() const
{
	if(!mShapes.empty())
	{
		b2Vec2 localCenter;

		if(mIsCircular)
		{
			b2CircleShape* circle = static_cast<b2CircleShape*>(mShapes[0].get());
			localCenter = circle->m_p;
		}
		else
		{
			std::vector<b2Vec2> centroids;

			for(std::size_t i = 0; i < mShapes.size(); i++)
			{
				b2Shape* shape = mShapes[i].get();
				// We can static cast here, since we know 100% it is a polygon shape.
				b2PolygonShape* polygon = static_cast<b2PolygonShape*>(shape);

				centroids.push_back(polygon->m_centroid); // The local position (compared to the whole geometry)
			}

			// Find average center
			for(auto &centroid : centroids)
			{
				localCenter.x += centroid.x;
				localCenter.y += centroid.y;
			}

			localCenter.x /= centroids.size();
			localCenter.y /= centroids.size();
		}

		return B2Vec2ToGlm(localCenter);
	}
	else
	{
		Utils::CRASH("Cannot get local 3D center of physics body that has no shapes! Please calculate them before calling.");
		return glm::vec2();
	}
}

// Very heavy! Involves reading from the GPU etc
glm::vec3 PhysicsBody::getShapesLocal3DCenter() const
{
	if(!mShapes.empty())
	{
		glm::vec2 localCenter = getShapesLocal2DCenter();

		// Find other coord center if object geometry is defined
		float otherCoord = 0.0f;

		if(mObjectGeometry)
		{
			ObjectGeometry::uintVector indices = mObjectGeometry->getIndexBuffer().readData();
			ObjectGeometry::vec3Vector positions3D = mObjectGeometry->getPositionBuffer().readData();

			std::size_t indexCount = indices.size();
													  // Convert polygons to 2D
			for(std::size_t i = 0; i < indexCount; i++)
			{
				unsigned int vertexIndex = indices[i]; // The index in the positions vector of this vertex
				glm::vec3 vertexPosition3D = positions3D[vertexIndex];

				otherCoord = vertexPosition3D.y;
			}
			otherCoord /= indexCount; // Find the average
		} else
		{
			// Ignore if it is not defined, it is most probably a simple circular shape with a radius
			otherCoord = 0.0f;
		}

		return glm::vec3(localCenter.x, otherCoord, localCenter.y);
	} else
	{
		Utils::CRASH("Cannot get local 3D center of physics body that has no shapes! Please calculate them before calling.");
		return glm::vec3();
	}
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

// Generates model matrix based on this body's position, rotation and scaling
glm::mat4 PhysicsBody::generateModelMatrix()
{
	glm::mat4 modelM = generateModelMatrix(
		getPosition(),
		getRotation(),
		mScaling);

	return modelM;
}

// timeStep in seconds, like Box2D (speed is in meters/seconds normally)
void PhysicsBody::step(float timeStep)
{
	mPosition.y += mVelocity.y * timeStep; // Add the missing velocity

	if(mWorldBody)
	{
		if(mWorldFriction != 0.0f)
		{
			float frictionIntensity = mWorldFriction * mWorldBody->GetMass();

			b2Vec2 linearVelocity = mWorldBody->GetLinearVelocity();
			float angularVelocity = mWorldBody->GetAngularVelocity();

			// Movement
			if(linearVelocity.x != 0.0f || linearVelocity.y != 0.0f)
			{
				// Multiply by velocity to avoid making objects continually go backwards

				// Opposite of movement
				b2Vec2 friction = b2Vec2(
					-linearVelocity.x * frictionIntensity,
					-linearVelocity.y * frictionIntensity);

				mWorldBody->ApplyForceToCenter(friction, true);
			}

			// Rotation
			if(!mIsFixtedRotation && angularVelocity != 0.0f)
			{
				float velocity = 0.0f;

				if(angularVelocity > 0.0f)
				{
					// Divide by 2 (guessed value), makes rotation deacceleration look better with movement deacceleration
					velocity = angularVelocity - frictionIntensity/2;
					if(velocity < 0.0f)
						velocity = 0.0f;
				} else
				{
					velocity = angularVelocity + frictionIntensity / 2;
					if(velocity > 0.0f)
						velocity = 0.0f;
				}


				mWorldBody->SetAngularVelocity(velocity); // Since SetTorque wasn't working for me
			}
		}

		float angleRadians = mWorldBody->GetAngle();
		// Angle expected to go over the limit soon, we have to do this check ourselves
		if(angleRadians > FLT_MAX - 100)
		{
			// Normalize angle
			// http://stackoverflow.com/questions/24234609/standard-way-to-normalize-an-angle-to-%CF%80-radians-in-java
			float normalizedAngle =
				angleRadians - CONST_TWO_PI * floor((angleRadians + CONST_PI) / CONST_TWO_PI);

			mWorldBody->SetTransform(mWorldBody->GetPosition(), angleRadians);
		}
	} // Ignore if it wasn't added, it might be a body of type Ignored!
}

// A quick an easy renderer for debugging
// Slow!
// Other 3D coord is the non-physics coord that the physics body will be dawn at
// Other 3D coord is useful if we want to draw all debug shapes on the same plane
// Takes a shader pointer to be consistent
// The shader is the same for basic objects
void PhysicsBody::renderDebugShape(constShaderPointer shader, const Camera* camera, float other3DCoord)
{
	glm::vec3 color(0.0f, 1.0f, 0.0f);

	if(mShapes.empty())
	{
		Utils::CRASH("Cannot debug render this physics body, it does not have shapes! Please calculate them before calling.");
		return;
	}
	
	// Lets do something very smart, create a buffer, fill it, and then delete it each time we render
	GPUBuffer<glm::vec3> positionBuffer;
	vec3Vector localPositions3D; // Object space coords, before model matrix!
	int drawMode = 0;

	if(mIsCircular)
	{
		// One shape per circular shape
		b2CircleShape* circle = static_cast<b2CircleShape*>(mShapes[0].get());

		// Translation is done in the model matrix
		// Put the circle on the object geometry shape (relative to the object geometry)
		vec2Vector vertices2D = getCircleVertices(getShapesLocal2DCenter(), circle->m_radius, 10);

		for(auto &vertex2D : vertices2D)
		{
			localPositions3D.push_back(glm::vec3(
				vertex2D.x * PHYSICS_PIXELS_PER_METER,
				0.0f,
				vertex2D.y * PHYSICS_PIXELS_PER_METER));
		}

		// Add line to see circle angle better

		// Front is the first element
		// We need to add this otherwise we see a hole in the circle
		localPositions3D.push_back(localPositions3D.front());
		localPositions3D.push_back(glm::vec3(0.0f)); // Add the center of the circle

		drawMode = GL_LINE_STRIP;
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
				localPositions3D.push_back(glm::vec3(
					point2D.x * PHYSICS_PIXELS_PER_METER,
					0.0f,
					point2D.y * PHYSICS_PIXELS_PER_METER));
			}
		}

		drawMode = GL_TRIANGLES;
		// Draw lines only, but they are still rasterized as triangles
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Must reset this after rendering!
	}

	positionBuffer.setMutableData(localPositions3D, GL_STATIC_DRAW);

	glm::vec3 position = getPosition();
	glm::mat4 modelMatrix = generateModelMatrix(
		glm::vec3(position.x, other3DCoord, position.z),
		glm::vec3(0.0f, getRotation().y, 0.0f), // Ignore any rotation apart Box2D's rotation
		glm::vec3(1.0f)); // No scaling here! The scaling is built-in the vertices

	glm::mat4 MVP = camera->getProjectionMatrix() * camera->getViewMatrix() * modelMatrix;

	glUseProgram(shader->getID());
	glUniformMatrix4fv(shader->findUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniform3f(shader->findUniform("color"), color.r, color.g, color.b);

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

	glDisable(GL_CULL_FACE); // Must re-enable after!

	// Draw!
	glDrawArrays(
		drawMode, // Mode
		0, // First
		localPositions3D.size() // Count
		);

	glDisableVertexAttribArray(0);
	glPolygonMode(GRAPHICS_RASTERIZE_FACE, GRAPHICS_RASTERIZE_MODE); // Reset
	glEnable(GL_CULL_FACE);
}

// Will use the body's position
// Of course, this means the debug shape will appear at the same place as the object geometry height
// (which is not necessarily the actual object's height)
void PhysicsBody::renderDebugShape(constShaderPointer shader, const Camera* camera)
{
	renderDebugShape(shader, camera, mPosition.y);
}