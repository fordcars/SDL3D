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

// ObjectGeometry

ObjectGeometry::ObjectGeometry(const std::string& name, const vec3Vector& vertices, const vec2Vector& UVs, const vec3Vector& normals)
{
	mName = name;
	
	mVertexBuffer.setMutableData(vertices, GL_DYNAMIC_DRAW); // DYNAMIC_DRAW as a hint to OpenGL that we might change the vertices
	mUVBuffer.setMutableData(UVs, GL_DYNAMIC_DRAW);
	mNormalBuffer.setMutableData(normals, GL_DYNAMIC_DRAW);
}

ObjectGeometry::~ObjectGeometry()
{
	// Do nothing
}

std::string ObjectGeometry::getName() const
{
	return mName;
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