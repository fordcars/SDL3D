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

#ifndef PHYSICS_BODY_HPP
#define PHYSICS_BODY_HPP

#include <Box2D.h>

#include <ObjectGeometry.hpp>

#include <memory>
#include <vector>
#include <string>

class PhysicsBody
{
private:
	using constObjectGeometryPointer = std::shared_ptr<const ObjectGeometry>;
	using b2ShapeUniquePointer = std::unique_ptr<b2Shape>; // Smart pointers mean ownership!!
	using b2Vec2Vector = std::vector<b2Vec2>;

	b2Body* mBody;
	b2ShapeUniquePointer mShape; // mShape will be able to hold different shape types

	constObjectGeometryPointer mObjectGeometry;
	bool mCircularShape;
	float mRadius; // Holds the radius if it is a circular shape
	int mType;

	// Static functions
	static b2ShapeUniquePointer createShapeFromObjectGeometry(const ObjectGeometry& objectGeometry, bool generateCircular);
	static b2ShapeUniquePointer createShapeFromRadius(float radius);
	static b2Vec2Vector get2DCoordsObjectGeometry(const ObjectGeometry& objectGeometry, bool topdown);

	static b2Vec2Vector monotoneChainConvexHull(b2Vec2Vector points2D);
	static b2Vec2 getCentroid(const b2Vec2Vector& points2D);
	static float getCircleRadius(b2Vec2Vector points2D, b2Vec2 centroid);

	static float cross(const b2Vec2& O, const b2Vec2& A, const b2Vec2& B);

	static glm::vec2 PhysicsBody::b2Vec2ToGlm(const b2Vec2& vec);
	static b2Vec2 PhysicsBody::glmToB2Vec2(const glm::vec2& vec);

public:
	PhysicsBody(constObjectGeometryPointer objectGeometry, bool circularShape, int type);
	PhysicsBody(const PhysicsBody& other);
	~PhysicsBody();

	void calculateShape();
	bool calculateShape(bool circularShape);
	bool calculateShape(bool circularShape, float radius);

	bool isCircularShape();
	float getRadius();

	bool addToWorld(b2World& world, glm::vec2 position, float density);
};

#endif /* PHYSICS_BODY_HPP */