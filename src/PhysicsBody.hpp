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

// Please only use glm::vecs in the public interface! No b2Vec2 here.

#ifndef PHYSICS_BODY_HPP
#define PHYSICS_BODY_HPP

#include <Box2D.h>
#include <Definitions.hpp>
#include <ObjectGeometry.hpp>

#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <string>

class PhysicsBody
{
private:
	using b2ShapeUniquePointer = std::unique_ptr<b2Shape>; // Smart pointers mean ownership!!
	using b2Vec2Vector = std::vector<b2Vec2>;

	b2Body* mWorldBody; // Needs to be removed from the world if it exists! Is null if this physics body is not in a world.

	b2ShapeUniquePointer mShape; // mShape will be able to hold different shape types, this is why it is a pointer
	// Don't destroy these:
	b2World* mWorld; // Keeps track of the world this body was added to
	ObjectGeometry* mObjectGeometry; // Keep track of the object geometry if necessairy

	// Can change dynamically
	glm::vec3 mPosition; // You need to use a getter to get the real value
	glm::vec3 mRotation;  // You need to use a getter to get the real value, in degrees
	float mDensity;
	float mFriction;
	int mType;

	glm::vec3 mScaling; // Can only change when recalculating
	bool mCircularShape; // Can only change when recalculating
	float mRadius; // Can only change when recalculating, holds the radius if it is a circular shape

	int mPixelsPerMeter; // Can't change

	// Static functions
	static b2ShapeUniquePointer createShapeFromObjectGeometry(const ObjectGeometry& objectGeometry,
		bool generateCircular, int pixelsPerMeter, glm::vec3 scaling);
	static b2ShapeUniquePointer createShapeFromRadius(float radius);
	static b2Vec2Vector get2DCoordsObjectGeometry(const ObjectGeometry& objectGeometry,
		int pixelsPerMeter, glm::vec3 scaling);

	static b2Vec2Vector monotoneChainConvexHull(b2Vec2Vector points2D);
	static b2Vec2 getCentroid(const b2Vec2Vector& points2D);
	static float getCircleRadius(b2Vec2Vector points2D, b2Vec2 centroid);

	static float cross(const b2Vec2& O, const b2Vec2& A, const b2Vec2& B);

	// Inline functions have to be defined in the header?
	static inline glm::vec2 PhysicsBody::b2Vec2ToGlm(const b2Vec2& vec) {return glm::vec2(vec.x, vec.y);}
	static inline b2Vec2 PhysicsBody::glmToB2Vec2(const glm::vec2& vec) { return b2Vec2(vec.x, vec.y); }

	static inline float radiansToDegrees(float radians) {return radians * (180.0f / CONST_PI);}
	static inline float degreesToRadians(float degrees) { return degrees * (CONST_PI / 180.0f); }

	bool updateBodyFixture();
	b2FixtureDef generateFixtureDefAndSetBodyDef(b2BodyDef& bodyDef);

public:
	PhysicsBody();
	PhysicsBody(ObjectGeometry* objectGeometry, bool circularShape, int type);
	PhysicsBody(const PhysicsBody& other);
	~PhysicsBody();

	bool calculateShape();
	bool calculateShape(bool circularShape, glm::vec3 scaling);
	bool calculateShape(bool circularShape, float radius);

	void setDensity(float density);
	float getDensity();
	void setFriction(float friction);
	float getFriction();

	bool isCircularShape();
	float getRadius();
	bool setType(int type);
	int getType();

	bool setPosition(glm::vec3 position);
	glm::vec3 getPosition();

	bool setRotation(glm::vec3 angle);
	glm::vec3 getRotation();
	glm::vec3 getRotationInRadians();

	bool setVelocity(glm::vec3 velocity);
	glm::vec3 getVelocity();

	bool addToWorld(b2World& world, glm::vec2 position, float density);
	glm::mat4 generateModelMatrix();
};

#endif /* PHYSICS_BODY_HPP */