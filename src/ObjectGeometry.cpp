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

#include <ObjectGeometry.hpp>
#include <Utils.hpp> // For vector stuff and error messages

#include <sstream>
#include <fstream> // For file stuff

ObjectGeometry::ObjectGeometry(const std::string& name,  vec3Vector& vertices, vec2Vector& UVs, vec3Vector& normals)
{
	mName = name;
	
	mVertexBuffer.setMutableData(vertices, GL_DYNAMIC_DRAW); // DYNAMIC_DRAW as a hint to OpenGL that we might change the vertices
	mUVBuffer.setMutableData(UVs, GL_DYNAMIC_DRAW);
	mNormalBuffer.setMutableData(normals, GL_DYNAMIC_DRAW);
}

// Keeps track of all of the object's data
ObjectGeometry::ObjectGeometry(const std::string& name, const std::string& objectPath) // Uses std::array for modernism, wasn't necessairy
{
	mName = name;
	
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> UVs;
	std::vector<glm::vec3> normals;
	loadOBJObject(objectPath, vertices, UVs, normals);

	// Keep track of all data

	mVertexBuffer.setMutableData(vertices, GL_DYNAMIC_DRAW);
	mUVBuffer.setMutableData(UVs, GL_DYNAMIC_DRAW);
	mNormalBuffer.setMutableData(normals, GL_DYNAMIC_DRAW);
}

ObjectGeometry::~ObjectGeometry()
{
	// Do nothing
}

// Static
// A super-simple .obj loader
bool ObjectGeometry::loadOBJObject(const std::string& filePath,
						 std::vector<glm::vec3>& outVertices,
						 std::vector<glm::vec2>& outUVs,
						 std::vector<glm::vec3>& outNormals)
{
	std::vector<unsigned int> vertexIndices, UVIndices, normalIndices; // Indices only, so ints

	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec2> tempUVs;
	std::vector<glm::vec3> tempNormals;

	std::ifstream file(filePath);
	std::string line;
	
	if(!file)
	{
		Utils::crash("The object " + filePath + " does not exist or cannot be opened!", __LINE__, __FILE__);
		return false;
	}
	
	while(std::getline(file, line)) // Give the line string so that getLine() can fill it
	{
		std::vector<std::string> words = Utils::splitString(line, ' ');

		if(words[0] == "v")
		{
			glm::vec3 vertex(std::stof(words[1]), std::stof(words[2]), std::stof(words[3]));
			tempVertices.push_back(vertex);
		} else if(words[0] == "vt")
		{
			glm::vec2 UV(std::stof(words[1]), std::stof(words[2]));
			tempUVs.push_back(UV);
		} else if(words[0] == "vn")
		{
			glm::vec3 normal(std::stof(words[1]), std::stof(words[2]), std::stof(words[3]));
			tempNormals.push_back(normal);
		} else if(words[0] == "f")
		{
			std::vector<std::string> vertex0 = Utils::splitString(words[1], '/');
			std::vector<std::string> vertex1 = Utils::splitString(words[2], '/');
			std::vector<std::string> vertex2 = Utils::splitString(words[3], '/');
			
			if(vertex0.size() != 3 || vertex1.size() != 3 || vertex2.size() != 3)
			{
				Utils::crash("Object at " + filePath + " is badly formatted for our simple loader!", __LINE__, __FILE__);
				return false;
			}
			
			vertexIndices.push_back(std::stoi(vertex0[0]));
			UVIndices.push_back(std::stoi(vertex0[1]));
			normalIndices.push_back(std::stoi(vertex0[2]));

			vertexIndices.push_back(std::stoi(vertex1[0]));
			UVIndices.push_back(std::stoi(vertex1[1]));
			normalIndices.push_back(std::stoi(vertex1[2]));

			vertexIndices.push_back(std::stoi(vertex2[0]));
			UVIndices.push_back(std::stoi(vertex2[1]));
			normalIndices.push_back(std::stoi(vertex2[2]));
		}
	}
	
	for(unsigned int i=0; i<vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int UVIndex = UVIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// The actual vertex for this one
		glm::vec3 vertex = tempVertices[vertexIndex-1]; // -1 since .obj indexes are 1-based
		outVertices.push_back(vertex);

		glm::vec2 UV = tempUVs[UVIndex-1];
		outUVs.push_back(UV);

		glm::vec3 normal = tempNormals[normalIndex-1];
		outNormals.push_back(normal);
	}

	return true;
}

ObjectGeometry::vec3Buffer& ObjectGeometry::getVertexBuffer()
{
	return mVertexBuffer;
}

ObjectGeometry::vec2Buffer& ObjectGeometry::getUVBuffer()
{
	return mUVBuffer;
}

ObjectGeometry::vec3Buffer& ObjectGeometry::getNormalBuffer()
{
	return mNormalBuffer;
}