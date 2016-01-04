//// Copyright 2015 Carl Hewett
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

#include <ObjectGeometryGroup.hpp>

#include <sstream>
#include <fstream> // For file stuff
#include <tiny_obj_loader.h>

#include <Utils.hpp> // For vector stuff and error messages
#include <ResourceManager.hpp> // For getting the basename of files

#include <glm/gtc/type_ptr.hpp> //////////////////FOR TESTING

ObjectGeometryGroup::ObjectGeometryGroup(const std::string& name)
{
	mName = name;
	mGeneratedNames = 0;
}

ObjectGeometryGroup::ObjectGeometryGroup(const std::string& name, const std::string& objectGeometryGroupFile,
										 bool splitGeometries)
{
	mName = name;
	mGeneratedNames = 0;

	loadOBJFile(objectGeometryGroupFile, splitGeometries);
}

ObjectGeometryGroup::~ObjectGeometryGroup()
{
	// Do nothing
}

void ObjectGeometryGroup::loadOBJFile(const std::string& OBJfilePath, bool splitGeometries)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	ObjectGeometry::uintVector indices;

	ObjectGeometry::vec3Vector vertices;
	ObjectGeometry::vec2Vector UVcoords;
	ObjectGeometry::vec3Vector normals;

	std::string tinyobjError = "";
	std::string materialBasePath = ""; // Same base path as the object

	// First, load the file stream
	std::ifstream file(OBJfilePath);

	if(!file)
	{
		Utils::CRASH("Object file '" + OBJfilePath + "' could not be opened!");
		return;
	}

	// Now load the object
	tinyobj::MaterialFileReader materialFileReader(materialBasePath);
	bool success = tinyobj::LoadObj(shapes, materials, tinyobjError, OBJfilePath.c_str());

	// Failed
	if(!success)
	{
		Utils::LOGPRINT(".obj file '" + OBJfilePath + "' failed to load!");
		Utils::CRASH("Tinyobjloader message: " + tinyobjError); // The error 'could' be an empty string, just sayin'
		return;
	} else if(!tinyobjError.empty())
	{
		Utils::WARN("Tinyobjloader message: " + tinyobjError); // The file should still load
	}

	// Loop through all shapes in the file and add them to the group
	for(std::size_t i=0; i<shapes.size(); i++)
	{
		tinyobj::shape_t& currentShape = shapes[i];
		std::size_t numberOfCurrentVertices = currentShape.mesh.positions.size()/3; // Since positions are vec3

		std::size_t indexToAppendTo = 0; 

		if(splitGeometries)
		{
			// Resize vectors to the amount of glm::vecXs
			// Any elements beyond the new size are destroyed
			vertices.resize(numberOfCurrentVertices);
			UVcoords.resize(numberOfCurrentVertices);
			normals.resize(numberOfCurrentVertices);

			indexToAppendTo = 0; // We will append the new vertices to the vectors at this index
		} else
		{
			// If we're not splitting the geometries, resize the vector to accommodate the next vertices
			std::size_t oldSize = vertices.size();
			std::size_t newSize = vertices.size() + numberOfCurrentVertices;

			// Append new indices
			indices.insert(indices.end(), currentShape.mesh.indices.begin(), currentShape.mesh.indices.end());

			vertices.resize(newSize);
			UVcoords.resize(newSize);
			normals.resize(newSize);

			indexToAppendTo = oldSize; // Will append the new vertices right after the old ones
		}

		// Copy the data since I couldn't find a way to avoid it
		// &Vertices[0] is a standard way to get a pointer to the data, whatever the type (including glm::vec3!)
		// glm::vec3 is a struct of FLOATING_POINT numbers! Same as the loaded mesh.
		std::memcpy(&vertices[indexToAppendTo], &currentShape.mesh.positions[0],
			Utils::getSizeOfVectorData(currentShape.mesh.positions));

		std::memcpy(&UVcoords[indexToAppendTo], &currentShape.mesh.texcoords[0],
			Utils::getSizeOfVectorData(currentShape.mesh.texcoords));

		std::memcpy(&normals[indexToAppendTo], &currentShape.mesh.normals[0],
			Utils::getSizeOfVectorData(currentShape.mesh.normals));

		if(splitGeometries)
		{
			std::string name = getValidName(currentShape.name); // Make sure we have a unique name

			objectGeometryPointer objectGeometryPointer(new ObjectGeometry(name,
				currentShape.mesh.indices, vertices, UVcoords, normals));
			addObjectGeometry(objectGeometryPointer);
		}
	}

	if(!splitGeometries)
	{
		objectGeometryPointer objectGeometryPointer(new ObjectGeometry(mName,
			indices, vertices, UVcoords, normals));
		addObjectGeometry(objectGeometryPointer);
	}
}

// Checks if the name is available. If not, it will generate one.
// Use this before creating and adding a geometry object to store the right name
std::string ObjectGeometryGroup::getValidName(const std::string& name)
{
	// If the name is empty
	if(name.empty())
		return Utils::generateUniqueMapStringKey(mObjectGeometryMap, mGeneratedNames);

	// If the name is already used
	else if(mObjectGeometryMap.find(name) != mObjectGeometryMap.end())
		return Utils::generateUniqueMapStringKey(mObjectGeometryMap, mGeneratedNames, name);

	// If it is valid
	else
		return name;
}

// Adds an object geometry. Will use the object's name as the key, or generate one if it has no name.
// Object geometries are copied to make them easily modifiable. They are, however, stored as shared pointers for efficiency.
ObjectGeometryGroup::objectGeometryPointer ObjectGeometryGroup::addObjectGeometry(objectGeometryPointer objectGeometryPointer)
{
	std::string objectGeometryName = objectGeometryPointer->getName();

	// Generate a unique key for the geometry if it doesn't have a name
	if(objectGeometryName.empty())
	{
		objectGeometryName = Utils::generateUniqueMapStringKey(mObjectGeometryMap, mGeneratedNames);
	}

	objectGeometryMapPair objectGeometryPair(objectGeometryName, objectGeometryPointer);
	std::pair<objectGeometryMap::iterator, bool> newlyAddedPair = mObjectGeometryMap.insert(objectGeometryPair);
	
	if(newlyAddedPair.second == false) // It already exists in the map
	{
		std::string error = "Object geometry '" + objectGeometryName + "' already exists in group '" + mName +
			"' and can't be added again.";
		Utils::CRASH(error);

		return newlyAddedPair.first->second;
	}

	return newlyAddedPair.first->second; // Get the pair at pair.first, then the pointer at ->second
}

// Returns smart pointer (instead of a reference) since similar functions did
ObjectGeometryGroup::objectGeometryPointer ObjectGeometryGroup::findObjectGeometry(const std::string& objectGeometryName)
{
	objectGeometryMap::iterator got = mObjectGeometryMap.find(objectGeometryName);

	if(got == mObjectGeometryMap.end()) // end() is past-the-end element iterator, so not found in the map!
	{
		std::string error = "Object geometry group '" + objectGeometryName + "' not found!";;
		Utils::CRASH(error);
		return got->second;
	}
	
	return got->second;
}

// Returns a vector of pointers to all object geometries
// Great for iterating through all object geometries
ObjectGeometryGroup::objectGeometryVector ObjectGeometryGroup::getObjectGeometries()
{
	objectGeometryVector vector;
	Utils::constructVectorFromMap(mObjectGeometryMap, vector);

	return vector;
}