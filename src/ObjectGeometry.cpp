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

#include "ObjectGeometry.hpp"
#include "Utils.hpp" // For vector stuff and error messages

// ObjectGeometry

ObjectGeometry::ObjectGeometry(const std::string& name,
							   const uintVector& indices, const vec3Vector& positions, const vec2Vector& UVs, const vec3Vector& normals)
							   : mIndexBuffer(GL_ELEMENT_ARRAY_BUFFER) // A special type of buffer
{
	mName = name;
	
	// GL_STATIC_DRAW as a hint to OpenGL that we probably won't change the data
	mIndexBuffer.setMutableData(indices, GL_STATIC_DRAW);
	mPositionBuffer.setMutableData(positions, GL_STATIC_DRAW);
	mUVBuffer.setMutableData(UVs, GL_STATIC_DRAW);
	mNormalBuffer.setMutableData(normals, GL_STATIC_DRAW);
}

ObjectGeometry::~ObjectGeometry()
{
	// Do nothing
}

std::string ObjectGeometry::getName() const
{
	return mName;
}

ObjectGeometry::uintBuffer& ObjectGeometry::getIndexBuffer()
{
	return mIndexBuffer;
}
const ObjectGeometry::uintBuffer& ObjectGeometry::getIndexBuffer() const
{
	return mIndexBuffer;
}

ObjectGeometry::vec3Buffer& ObjectGeometry::getPositionBuffer()
{
	return mPositionBuffer;
}
const ObjectGeometry::vec3Buffer& ObjectGeometry::getPositionBuffer() const
{
	return mPositionBuffer;
}

ObjectGeometry::vec2Buffer& ObjectGeometry::getUVBuffer()
{
	return mUVBuffer;
}
const ObjectGeometry::vec2Buffer& ObjectGeometry::getUVBuffer() const
{
	return mUVBuffer;
}

ObjectGeometry::vec3Buffer& ObjectGeometry::getNormalBuffer()
{
	return mNormalBuffer;
}
const ObjectGeometry::vec3Buffer& ObjectGeometry::getNormalBuffer() const
{
	return mNormalBuffer;
}