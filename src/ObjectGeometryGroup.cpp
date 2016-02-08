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

#include <ObjectGeometryGroup.hpp>

#include <sstream>
#include <fstream> // For file stuff
#include <algorithm>
#include <cstddef> // For std::size_t
#include <tiny_obj_loader.h>

#include <Utils.hpp> // For vector stuff and error messages
#include <ResourceManager.hpp> // For getting the basename of files

ObjectGeometryGroup::ObjectGeometryGroup(const std::string& name)
{
	mName = name;
	mGeneratedNames = 0;
}

ObjectGeometryGroup::ObjectGeometryGroup(const std::string& name, const std::string& objectGeometryGroupFile)
{
	mName = name;
	mGeneratedNames = 0;

	loadOBJFile(objectGeometryGroupFile);
}

ObjectGeometryGroup::~ObjectGeometryGroup()
{
	// Do nothing
}

std::string ObjectGeometryGroup::getName()
{
	return mName;
}

// Loads an .obj file. The objects found will be added to this group.
bool ObjectGeometryGroup::loadOBJFile(const std::string& OBJfilePath)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string tinyobjError = "";
	std::string materialBasePath = ""; // Same base path as the object

	// First, load the file stream
	std::ifstream file(OBJfilePath);

	if(!file)
	{
		Utils::CRASH("Object file '" + OBJfilePath + "' could not be opened!");
		return false;
	}

	// Now load the object
	tinyobj::MaterialFileReader materialFileReader(materialBasePath);
	bool success = tinyobj::LoadObj(shapes, materials, tinyobjError, OBJfilePath.c_str());

	// Failed
	if(!success)
	{
		Utils::LOGPRINT(".obj file '" + OBJfilePath + "' failed to load!");
		Utils::CRASH("Tinyobjloader message: " + tinyobjError); // The error 'could' be an empty string, just sayin'
		return false;
	} else if(!tinyobjError.empty()) // Success, but there is a warning
	{
		Utils::WARN("Tinyobjloader message: " + tinyobjError); // The file should still load
	}

	// Loop through all shapes in the file and add them to the group
	for(std::size_t i=0; i<shapes.size(); i++)
	{
		tinyobj::shape_t& currentShape = shapes[i];

		// Check if we have invalid values (runtime errors suck)
		if(currentShape.mesh.positions.size()%3 != 0 ||
			currentShape.mesh.texcoords.size()%2 != 0 ||
			currentShape.mesh.normals.size()%3 != 0)
		{
			Utils::WARN("OBJ data for geometry '" + currentShape.name + "' in file '" + OBJfilePath +
				"' for group '" + mName + "' is invalid! Skipping this geometry.");
			continue;
		}

		if(currentShape.mesh.indices.size() % 3 != 0) // Not triangles, but Tinyobjloader should of taken care of this
		{
			Utils::WARN("OBJ data for geometry '" + currentShape.name + "' in file '" + OBJfilePath +
				"' for group '" + mName + "' does not contain triangles! Tinyobjloader should of taken care of this. Bug?" +
				" To fix this, open your model in a object modeling software, triangulate the faces and the export it as" + 
				" .obj (Wavefront).");
			continue;
		}

		std::size_t numberOfVertices = currentShape.mesh.positions.size()/3; // Since positions are vec3

		// Create the vectors with the right sizes
		ObjectGeometry::vec3Vector positions(numberOfVertices);
		ObjectGeometry::vec2Vector UVcoords(numberOfVertices);
		ObjectGeometry::vec3Vector normals(numberOfVertices);

		// Final safety check!
		if(currentShape.mesh.positions.size()/3 != currentShape.mesh.normals.size()/3 ||
			currentShape.mesh.positions.size()/3 != currentShape.mesh.texcoords.size()/2 ||
			currentShape.mesh.texcoords.size()/2 != currentShape.mesh.normals.size()/3)
		{
			Utils::CRASH("OBJ data for geometry '" + currentShape.name + "' in file '" + OBJfilePath +
				"' for group '" + mName + "' is not coherent! Did you include normals/texcoords and in the same amount?");
			return false;
		}

		// Copy the data since I couldn't find a way to avoid it
		// Very annoying to iterate through all vertices, but seems to be the safest
		for(std::size_t j=0; j<numberOfVertices; j++)
		{
			positions[j] = glm::vec3(currentShape.mesh.positions[j*3],            // X
										 currentShape.mesh.positions[j*3 + 1],    // Y
										 currentShape.mesh.positions[j*3 + 2]);   // Z

			UVcoords[j] = glm::vec2(currentShape.mesh.texcoords[j*2],             // X
										 currentShape.mesh.texcoords[j*2 + 1]);   // Y

			normals[j] = glm::vec3(currentShape.mesh.normals[j*3],                // X
										 currentShape.mesh.normals[j*3 + 1],      // Y
										 currentShape.mesh.normals[j*3 + 2]);     // Z
		}

		std::string name = getValidName(currentShape.name); // Make sure we have a unique name

		objectGeometryPointer objectGeometryPointer(new ObjectGeometry(name,
			currentShape.mesh.indices, positions, UVcoords, normals));
		addObjectGeometry(objectGeometryPointer);
	}

	return true; // Success!
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