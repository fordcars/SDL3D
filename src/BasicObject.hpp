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

#ifndef BASICOBJECT_HPP_
#define BASICOBJECT_HPP_

#include <ObjectGeometry.hpp>
#include <glm/glm.hpp>
#include <GLAD/glad.h> // OpenGL, rendering and all

class BasicObject
{
protected:
	ObjectGeometry& getObjectGeometry();

private:
	ObjectGeometry mObjectGeometry;
	ObjectGeometry::constShaderPointer mShaderPointer; // The shader used to render this object, pointer.

public:
	BasicObject(const ObjectGeometry& objectGeometry, ObjectGeometry::constShaderPointer shaderPointer);
	~BasicObject();

	void setShader(ObjectGeometry::constShaderPointer shaderPointer);
	ObjectGeometry::constShaderPointer getShader();

	virtual void render(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix); // Overload this if you need to!
};

#endif /* BASICOBJECT_HPP_ */