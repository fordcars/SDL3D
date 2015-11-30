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

// BaseObject: A simple textureless object, useful as a base class for more complex type

// In:
// - layout location 0: vertex position in modelspace

// Uniforms:
// - mat4 MVP

#include <BasicObject.hpp>

BasicObject::BasicObject(const ObjectGeometry& objectGeometry, ObjectGeometry::constShaderPointer shaderPointer)
	: mObjectGeometry(objectGeometry) // Copy the ObjectGeometry
{
	mShaderPointer = shaderPointer;
}

BasicObject::~BasicObject()
{
}

ObjectGeometry& BasicObject::getObjectGeometry()
{
	return mObjectGeometry;
}

ObjectGeometry::constShaderPointer BasicObject::getShader()
{
	return mShaderPointer;
}

 // Useful for changing the shader for different effects "on the fly"
void BasicObject::setShader(ObjectGeometry::constShaderPointer shaderPointer)
{
	mShaderPointer = shaderPointer;
}

// Virtual
void BasicObject::render(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

	ObjectGeometry::vec3Buffer& vertexBuffer = mObjectGeometry.getVertexBuffer();

	glUseProgram(mShaderPointer->getID());
	glUniformMatrix4fv(mShaderPointer->findUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

	glEnableVertexAttribArray(0); // Number to give to OpenGL VertexAttribPointer
	vertexBuffer.bind(GL_ARRAY_BUFFER);

	// Give it to the shader. Each time the vertex shader runs, it will get the next element of this buffer.
	glVertexAttribPointer(
		0,					// Attribute 0, no particular reason but same as the vertex shader's layout and glEnableVertexAttribArray
		3,					// Size. Number of values per vertex, must be 1, 2, 3 or 4.
		GL_FLOAT,			// Type of data (GLfloats)
		GL_FALSE,			// Normalized?
		0,					// Stride
		(void*)0			// Array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.getLength()); // Draw!
	glDisableVertexAttribArray(0);
}
