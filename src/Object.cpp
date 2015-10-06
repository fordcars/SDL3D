// Copyright 2015 Carl Hewett

// This file is part of SDL3D.

// SDL3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// SDL3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.

#include <Object.hpp>
#include <HelperFunctions.hpp> // For vector stuff and error messages

#include <fstream> // For file stuff

// Uniforms:
// - mat4 MVP

// In:
// - vertex position in modelspace

Object::Object(GLfloatVector &vertices, shaderPointer shaderPointer) // Uses std::array for modernism, wasn't necessairy
{
	mShaderPointer = shaderPointer;

	mVertexBuffer.bind(GL_ARRAY_BUFFER);
	mVertexBuffer.setMutableData(GL_ARRAY_BUFFER, vertices, GL_DYNAMIC_DRAW); // DYNAMIC_DRAW as a hint to OpenGL that we might change the vertices
}

Object::~Object()
{
	// Do nothing
}

// Static
void Object::loadOBJData(const std::string& filePath)
{

}

Object::GlfloatBuffer &Object::getVertexBuffer()
{
	return mVertexBuffer;
}

void Object::setShader(shaderPointer shaderPointer)
{
	mShaderPointer = shaderPointer;
}

Object::shaderPointer Object::getShader()
{
	return mShaderPointer;
}

void Object::render(glm::mat4 MVP)
{
	glUseProgram(mShaderPointer->getID());
	glUniformMatrix4fv(mShaderPointer->findUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

	glEnableVertexAttribArray(0); // Number to give to OpenGL VertexAttribPointer
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer.getID()); // All future function calls will modify this vertex buffer

	// Give it to the shader. Each time the vertex shader runs, it will get the next element of this buffer.
	glVertexAttribPointer(
		0,					// Attribute 0, no particular reason but same as the vertex shader's layout and glEnableVertexAttribArray
		3,					// Size. Number of values per vertex, must be 1, 2, 3 or 4.
		GL_FLOAT,			// Type of data (GLfloats)
		GL_FALSE,			// Normalized?
		0,					// Stride
		(void*)0			// Array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, mVertexBuffer.getSize(GL_ARRAY_BUFFER)); // Draw!
	glDisableVertexAttribArray(0);
}
