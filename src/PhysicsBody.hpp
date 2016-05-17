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

#include <Definitions.hpp>
#include <ObjectGeometry.hpp>

#include <Box2D.h>

#include <poly2tri.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <string>

class Shader;
class Camera;
class PhysicsBody
{
private:
	using shapeUniquePointer = std::unique_ptr<b2Shape>; // Smart pointers mean ownership!!
	using shapeVector = std::vector<shapeUniquePointer>;
	using fixtureDefVector = std::vector<b2FixtureDef>;

	using constObjectGeometryPointer = std::shared_ptr<const ObjectGeometry>;
	using constShaderPointer = std::shared_ptr<const Shader>;

	using B2Vec2Vector = std::vector<b2Vec2>; // B2 to not be mistaken with b2
	using vec2Vector = std::vector<glm::vec2>;
	using vec3Vector = std::vector<glm::vec3>;

	void init();

	// Will be able to hold different Box2D shapes, this is why it is a pointer
	// This will hold the shape of this body. If you want to modify it in the world, get the shape from the world!
	// This is only a local copy
	shapeVector mShapes;

	// Needs to be removed from the world whendeconstructing if it exists! Is null if this physics body is not in a world.
	b2Body* mWorldBody;

	// Don't destroy these:
	b2World* mWorld; // Keeps track of the world this body was added to
	constObjectGeometryPointer mObjectGeometry; // Keep track of the object geometry if necessairy

	// Can change dynamically
	// You need to use a getter to get the real value
	// These members will be used to add to a world, etc.
	glm::vec3 mPosition;
	glm::vec3 mRotation;  // In degrees, will need to recalculate if you want the shape to refelct the rotation
	glm::vec3 mVelocity;
	bool mIsBullet;
	bool mIsFixtedRotation;

	// Can change dynamically
	float mDensity;
	float mFriction;
	float mRestitution;
	float mWorldFriction; // This is multiplied by the mass. Use this for topdown physics, for example.

	glm::vec3 mScaling; // Can only change when calculating (since Box2D doesn't support chaning scaling dynamically)
	bool mIsCircular; // Can only change when calculating
	float mRadius; // Can only change when calculating, holds the radius if it is a circular shape

	int mType; // Can't change

	// Static functions
	static shapeVector createShapesFromObjectGeometry(const ObjectGeometry& objectGeometry,
		bool generateCircular, float pixelsPerMeter, glm::vec3 rotation, glm::vec3 scaling);
	static shapeUniquePointer createShapesFromRadius(float radius);
	static B2Vec2Vector get2DObjectGeometryCoords(const ObjectGeometry& objectGeometry,
		float pixelsPerMeter, glm::vec3 rotation, glm::vec3 scaling);

	static std::vector<p2t::Point> monotoneChainConvexHull(B2Vec2Vector points2D);
	static b2Vec2 get2DCentroid(const B2Vec2Vector& points2D);
	static float getCircleRadiusFromPoints(B2Vec2Vector points2D, b2Vec2 centroid);

	static std::vector<p2t::Point*> pointerizeP2t(const std::vector<p2t::Point>& points);

	static inline float cross(const p2t::Point& O, const p2t::Point& A, const p2t::Point& B)
	{
		return static_cast<float>((A.x - O.x)*(B.y - O.y) - (A.y - O.y)*(B.x - O.x));
	}

	static inline float distanceSquared(const p2t::Point& A, const p2t::Point& B)
	{
		return static_cast<float>((B.x - A.x)*(B.x - A.x) + (B.y - A.y)*(B.y - A.y));
	}

	static vec2Vector getCircleVertices(glm::vec2 origin, float radius, int angleIncrementation);
	static glm::mat4 generateModelMatrix(glm::vec3 pixelTranslation, glm::vec3 rotation, glm::vec3 scaling);

	// Inline functions have to be defined in the header?
	static inline glm::vec2 B2Vec2ToGlm(const b2Vec2& vec) {return glm::vec2(vec.x, vec.y);}
	static inline b2Vec2 glmToB2Vec2(const glm::vec2& vec) { return b2Vec2(vec.x, vec.y); }

	static inline b2Vec2 p2tToB2Vec2(const p2t::Point& vec) { return b2Vec2(static_cast<float>(vec.x), static_cast<float>(vec.y)); }
	static inline p2t::Point B2Vec2ToP2t(const b2Vec2& vec) { return p2t::Point(vec.x, vec.y); }

	static inline float radiansToDegrees(float radians) {return radians * (180.0f / CONST_PI);}
	static inline float degreesToRadians(float degrees) { return degrees * (CONST_PI / 180.0f); }

	static inline float getVectorNorm(const b2Vec2& vector)
	{
		return sqrt((vector.x * vector.x) + (vector.y * vector.y));
	}
	static inline float getSquaredVectorNorm(const b2Vec2& vector)
	{
		return (vector.x * vector.x) + (vector.y * vector.y);
	}

	static inline float getDistanceBetweenPointsSquared(const b2Vec2& point1, const b2Vec2& point2)
	{
		return (point2.x - point1.x)*(point2.x - point1.x) + (point2.y - point1.y)*(point2.y - point1.y);
	}

	static inline float getDistanceBetweenPoints(const b2Vec2& point1, const b2Vec2& point2)
	{
		return sqrt((point2.x - point1.x)*(point2.x - point1.x) + (point2.y - point1.y)*(point2.y - point1.y));
	}

	fixtureDefVector generateFixtureDefsAndSetBodyDef(b2BodyDef& bodyDef);
	bool updateWorldBodyFixtures();

public:
	PhysicsBody();
	PhysicsBody(float radius, int type);
	PhysicsBody(constObjectGeometryPointer objectGeometry, bool circularShape, int type);
	PhysicsBody(const PhysicsBody& other);
	~PhysicsBody();

	void setObjectGeometry(constObjectGeometryPointer objectGeometry);

	bool calculateShapes();
	bool calculateShapes(bool isCircularShape, glm::vec3 scaling);
	bool calculateShapes(float radius);

	void setDensity(float density);
	float getDensity() const;
	void setFriction(float friction);
	float getFriction() const;
	void setRestitution(float restitution);
	float getRestitution() const;
	void setWorldFriction(float friction);
	float getWorldFriction() const;

	bool isCircular() const;
	float getRadius() const;
	int getType() const;

	void setPosition(glm::vec3 position);
	glm::vec3 getPosition() const;

	void setRotation(glm::vec3 angle);
	void setRotationInRadians(glm::vec3 angle);
	glm::vec3 getRotation() const;
	glm::vec3 getRotationInRadians() const;

	void setVelocity(glm::vec3 velocity);
	glm::vec3 getVelocity() const;

	void setBullet(bool isBullet);
	bool isBullet() const;
	void setFixtedRotation(bool fixted);
	bool isFixtedRotation() const;

	glm::vec2 getShapesLocal2DCenter() const;
	glm::vec3 getShapesLocal3DCenter() const;

	bool addToWorld(b2World* world);
	void removeFromWorld();

	glm::mat4 generateModelMatrix();

	void step(float timeStep);

	void renderDebugShape(constShaderPointer shader, const Camera* camera, float other3DCoord);
	void renderDebugShape(constShaderPointer shader, const Camera* camera);
};

#endif // PHYSICS_BODY_HPP
