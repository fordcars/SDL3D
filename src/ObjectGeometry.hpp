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

// This class holds the vertex data. Use this class as a member for other 3D objects.

#ifndef OBJECT_GEOMETRY_HPP_
#define OBJECT_GEOMETRY_HPP_

#include <memory> // For smart pointers

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include <Shader.hpp>
#include <GPUBuffer.hpp>

class ObjectGeometry
{
public:
	typedef GPUBuffer<unsigned int> uintBuffer;
	typedef GPUBuffer<glm::vec2> vec2Buffer;
	typedef GPUBuffer<glm::vec3> vec3Buffer;

	typedef std::vector<unsigned int> uintVector;
	typedef std::vector<glm::vec2> vec2Vector;
	typedef std::vector<glm::vec3> vec3Vector;

private:
	typedef std::shared_ptr<const Shader> constShaderPointer; // Const shader

	std::string mName; // Don't change this stupidly

	uintBuffer mIndexBuffer;
	vec3Buffer mPositionBuffer;
	vec2Buffer mUVBuffer;
	vec3Buffer mNormalBuffer;

public:
	ObjectGeometry(const std::string& name,
		const uintVector& indices, const vec3Vector& positions, const vec2Vector& UVs, const vec3Vector& normals);
	~ObjectGeometry();

	std::string getName() const;

	uintBuffer& getIndexBuffer();
	vec3Buffer& getPositionBuffer();
	vec2Buffer& getUVBuffer();
	vec3Buffer& getNormalBuffer();
};

#endif /* OBJECT_GEOMETRY_HPP_ */