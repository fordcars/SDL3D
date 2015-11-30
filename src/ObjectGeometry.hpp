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

// This class holds the vertices. Use this class as a member for other 3D objects.

#ifndef OBJECTGEOMETRY_HPP_
#define OBJECTGEOMETRY_HPP_

#include <memory> // For smart pointers

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include <Shader.hpp>
#include <GLBuffer.hpp>

class ObjectGeometry
{
public:
	typedef GLBuffer<glm::vec2> vec2Buffer;
	typedef GLBuffer<glm::vec3> vec3Buffer;
	typedef std::vector<glm::vec2> vec2Vector;
	typedef std::vector<glm::vec3> vec3Vector;
	typedef std::shared_ptr<const Shader> constShaderPointer; // Const shader

private:
	std::string mName;

	vec3Buffer mVertexBuffer; // The OpenGL vertex buffer
	vec2Buffer mUVBuffer;
	vec3Buffer mNormalBuffer;

public:
	ObjectGeometry(const std::string& name, vec3Vector& vertices, vec2Vector& UVs, vec3Vector& normals);
	ObjectGeometry(const std::string& name, const std::string& objectPath);
	~ObjectGeometry();

	static bool loadOBJObject(const std::string& filePath,
						 std::vector<glm::vec3>& outVertices,
						 std::vector<glm::vec2>& outUVs,
						 std::vector<glm::vec3>& outNormals);

	vec3Buffer& getVertexBuffer();
	vec2Buffer& getUVBuffer();
	vec3Buffer& getNormalBuffer();
};

#endif /* OBJECTGEOMETRY_HPP_ */