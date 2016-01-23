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

#ifndef PHYSICS_BODY
#define PHYSICS_BODY

#include <Box2D.h>

#include <ObjectGeometry.hpp>

#include <vector>
#include <string>

class PhysicsBody
{
private:
	b2Body* mBody;

	static b2Body* createStaticBodyFromObjectGeometry(const ObjectGeometry& objectGeometry);
	static b2Body* createDynamicBodyFromObjectGeometry(const ObjectGeometry& objectGeometry);
	static std::vector<b2Vec2> get2DBoundingBoxFromObjectGeometry(const ObjectGeometry& objectGeometry, bool topdown);

public:
	PhysicsBody(const ObjectGeometry& objectGeometry, int type);
	~PhysicsBody();
};

#endif /* PHYSICS_BODY */