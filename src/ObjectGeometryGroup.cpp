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

#include <glm/gtc/type_ptr.hpp> //////////////////FOR TESTING

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

void ObjectGeometryGroup::loadOBJFile(const std::string& OBJfilePath)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string tinyobjError;

	std::string materialBasePath = ""; // Same base path as the object. Is this the way to do it?

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
		Utils::CRASH("Tinyobjloader error message: " + tinyobjError); // The error 'could' be an empty string, just sayin'
		return;
	} else if(!tinyobjError.empty())
	{
		Utils::WARN("Tinyobjloader error message: " + tinyobjError); // The file should still load
	}

	// Loop through all shapes in the file and add them to the group
	for(size_t i=0; i<shapes.size(); i++)
	{
		tinyobj::shape_t& currentShape = shapes[i];

		// Annoyingly long but pretty efficient
		size_t numberOfVertexValues = currentShape.mesh.positions.size();
		size_t numberOfUVValues = currentShape.mesh.texcoords.size();
		size_t numberOfNormalValues = currentShape.mesh.normals.size();
		
		//////////////DEBUG
		/*Utils::LOGPRINT("Number of indices: " + std::to_string(currentShape.mesh.indices.size()));
		Utils::LOGPRINT("-------------Vertex positions for: '" + currentShape.name + "'-------------");
		for(size_t j=0; j<numberOfVertexValues/3; j++)
		{
			Utils::LOGPRINT("x: " + std::to_string(currentShape.mesh.positions[j*3]) + ",");
			Utils::LOGPRINT("y: " + std::to_string(currentShape.mesh.positions[j*3+1]) + ",");
			Utils::LOGPRINT("z: " + std::to_string(currentShape.mesh.positions[j*3+2]));

			Utils::LOGPRINT("\n");
		}
		Utils::LOGPRINT("-------------End vertex positions-------------");*/


		//Utils::LOGPRINT("-------------Vertex positions for: '" + currentShape.name + "'-------------");

		//////////////DEBUG

		ObjectGeometry::vec3Vector vertices(currentShape.mesh.indices.size());
		ObjectGeometry::vec2Vector UVcoords(currentShape.mesh.indices.size());
		ObjectGeometry::vec3Vector normals(currentShape.mesh.indices.size());

		// For testing purposes, iterate through the indexes to get positions, UVs, etc. Normally we would use OpenGL VBO indexing for this.
		for(size_t j=0; j<currentShape.mesh.indices.size(); j++)
		{
			// Hahah, this gets the vertex data at the index found and transforms it into a vec3 or vec2
			vertices[j] = glm::make_vec3(&(  currentShape.mesh.positions[  currentShape.mesh.indices[j] *3  ])); // *3 for 3 values

			/*Utils::LOGPRINT("Index: " + std::to_string(currentShape.mesh.indices[j]));
			Utils::LOGPRINT("x: " + std::to_string(vertices[j].x));
			Utils::LOGPRINT("y: " + std::to_string(vertices[j].y));
			Utils::LOGPRINT("z: " + std::to_string(vertices[j].z));

			Utils::LOGPRINT("\n");

			if((j+1)%3 == 0) // A triangle
				Utils::LOGPRINT("-\n\n\n");*/

			UVcoords[j] = glm::make_vec2(&(  currentShape.mesh.texcoords[  currentShape.mesh.indices[j] *2 ])); // *2 for 2 values
			normals[j] = glm::make_vec3(&(  currentShape.mesh.normals[  currentShape.mesh.indices[j] *3 ])); // *3 for 3 values
		}


		/*float* firstVertice = &currentShape.mesh.positions[0];
		float* firstUV = &currentShape.mesh.texcoords[0];
		float* firstNormal = &currentShape.mesh.normals[0];

		ObjectGeometry::vec3Vector vertices(firstVertice, firstVertice + numberOfVertexValues);
		ObjectGeometry::vec2Vector UVcoords(firstUV, firstUV + numberOfUVValues);
		ObjectGeometry::vec3Vector normals(firstNormal, firstNormal + numberOfNormalValues);*/

		std::string name = getValidName(currentShape.name); // Make sure we have a unique name

Utils::LOGPRINT("ObjectGeometry name: '" + name);//// DEBUGGGGG

		objectGeometryPointer objectGeometryPointer(new ObjectGeometry(name, vertices, UVcoords, normals));
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
		std::string error = "Object geometry '" + objectGeometryName + "' already exists in group '" + mName + "'. Try using 'getValidName()' before creating the object geometry!";
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