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

// A simple textureless object. Use it as a base class for different types of objects.
// We only support triangles right now!

// In:
// - layout location 0: vertex position in modelspace

// Uniforms:
// - mat4 MVP

#include <Object.hpp>

// Objects copy objectGeometry instead of pointing to them, allow you to modify them
Object::Object(constObjectGeometryPointer objectGeometry, constShaderPointer shaderPointer,
	bool physicsCircularShape, int physicsType)
	: Entity(objectGeometry, physicsCircularShape, physicsType), mObjectGeometry(objectGeometry) // Copy the ObjectGeometry
{
	mShaderPointer = shaderPointer;
}

Object::~Object()
{
}

void Object::setObjectGeometry(constObjectGeometryPointer objectGeometry)
{
	mObjectGeometry = objectGeometry;
	getPhysicsBody().setObjectGeometry(objectGeometry);
}

Object::constObjectGeometryPointer Object::getObjectGeometry() const
{
	return mObjectGeometry;
}

 // Useful for changing the shader for different effects "on the fly"
void Object::setShader(constShaderPointer shaderPointer)
{
	mShaderPointer = shaderPointer;
}

Object::constShaderPointer Object::getShader() const
{
	return mShaderPointer;
}

// Virtual
void Object::render(const Camera& camera)
{
	const ObjectGeometry::uintBuffer& indexBuffer = mObjectGeometry->getIndexBuffer();
	const ObjectGeometry::vec3Buffer& positionBuffer = mObjectGeometry->getPositionBuffer();

	glm::mat4 modelMatrix = getPhysicsBody().generateModelMatrix();
	glm::mat4 MVP = camera.getProjectionMatrix() * camera.getViewMatrix() * modelMatrix;

	glUseProgram(mShaderPointer->getID());
	glUniformMatrix4fv(mShaderPointer->findUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

	glEnableVertexAttribArray(0); // Number to give to OpenGL VertexAttribPointer
	positionBuffer.bind(GL_ARRAY_BUFFER);

	// Give it to the shader. Each time the vertex shader runs, it will get the next element of this buffer.
	glVertexAttribPointer(
		0,					// Attribute 0, no particular reason but same as the vertex shader's layout and glEnableVertexAttribArray
		3,					// Size. Number of values per vertex, must be 1, 2, 3 or 4.
		GL_FLOAT,			// Type of data (GLfloats)
		GL_FALSE,			// Normalized?
		0,					// Stride
		(void*)0			// Array buffer offset
	);

	// Draw!
	// Use the index buffer, more efficient!
	glDrawElements(
		GL_TRIANGLES,            // Mode
		indexBuffer.getLength(), // Count
		GL_UNSIGNED_INT,         // Type
		(void*)0                 // Element array buffer offset
	);

	glDisableVertexAttribArray(0);
}
