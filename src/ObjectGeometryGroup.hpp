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

#ifndef OBJECT_GEOMETRY_GROUP_HPP
#define OBJECT_GEOMETRY_GROUP_HPP

#include "ObjectGeometry.hpp"

#include <memory> // For smart pointers
#include <string>
#include <vector>
#include <map>

// Fancy! You can group object geometries together. Ex: levels, complex objects, animations, etc.
class ObjectGeometryGroup
{
public:
	using objectGeometryPointer  = std::shared_ptr<ObjectGeometry>;
	using objectGeometryMap      = std::map<std::string, objectGeometryPointer>;
	using objectGeometryMapPair  = std::pair<std::string, objectGeometryPointer>;

	using objectGeometryVector = std::vector<objectGeometryPointer>;

private:
	std::string mName;
	objectGeometryMap mObjectGeometryMap;

	int mGeneratedNames; // For generating unique logical geometry names if needed

	bool loadOBJFile(const std::string& OBJfilePath);

public:
	ObjectGeometryGroup(const std::string& name);
	ObjectGeometryGroup(const std::string& name, const std::string& objectFile);
	~ObjectGeometryGroup();

	std::string getName();

	std::string getValidName(const std::string& objectGeometryName);

	// In public interface so we can create objectGeometries from hardcoded data, etc
	objectGeometryPointer addObjectGeometry(objectGeometryPointer objectGeometryPointer);

	objectGeometryPointer findObjectGeometry(const std::string& objectGeometryName);
	objectGeometryVector getObjectGeometries();
};

#endif // OBJECT_GEOMETRY_GROUP_HPP