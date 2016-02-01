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

// This class holds the vertex data. Use this class as a member for other 3D objects.

#ifndef OBJECT_GEOMETRY_HPP
#define OBJECT_GEOMETRY_HPP

#include <memory> // For smart pointers

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include <Shader.hpp>
#include <GPUBuffer.hpp>

class ObjectGeometry
{
public:
	using uintBuffer = GPUBuffer<unsigned int>;
	using vec2Buffer = GPUBuffer<glm::vec2>;
	using vec3Buffer = GPUBuffer<glm::vec3>;

	using uintVector = std::vector<unsigned int>;
	using vec2Vector = std::vector<glm::vec2>;
	using vec3Vector = std::vector<glm::vec3>;

private:
	using constShaderPointer = std::shared_ptr<const Shader>; // Const shader

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

	// Return a const buffer if we need it, could be useful
	uintBuffer& getIndexBuffer();
	const uintBuffer& getIndexBuffer() const;

	vec3Buffer& getPositionBuffer();
	const vec3Buffer& getPositionBuffer() const;

	vec2Buffer& getUVBuffer();
	const vec2Buffer& getUVBuffer() const;

	vec3Buffer& getNormalBuffer();
	const vec3Buffer& getNormalBuffer() const;
};

#endif /* OBJECT_GEOMETRY_HPP */