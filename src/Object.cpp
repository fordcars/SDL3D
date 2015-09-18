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

#include <Object.h>
#include <HelperFunctions.h>

// Uniforms:
// - mat4 MVP

// In:
// - vertex position in modelspace

Object::Object(GLfloatArray vertices, int numberOfVertices, shaderPointer shader) // Uses std::array for modernism, wasn't necessairy
{
	mNumberOfVertices = numberOfVertices;
	mShader = shader;
	
	int vertexDataSize = sizeof(GLfloat) * numberOfVertices * 3; // Calculate array size
	GLfloat *vertexData = vertices.data();

	// Create a VBO. Done once per object
	glGenBuffers(1, &mVertexBuffer); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer); // Say it's an array
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, GL_STATIC_DRAW); // Give it to OpenGL
}

Object::~Object()
{
	// Do nothing
}

void Object::setShader(shaderPointer shader)
{
	mShader = shader;
}

Object::shaderPointer Object::getShader()
{
	return mShader;
}

void Object::render(glm::mat4 MVP)
{
	glUseProgram(mShader->getID());
	glUniformMatrix4fv(mShader->findUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer); // All future function calls will modify this vertex buffer

	// Give it to the shader. Each time the vertex shader runs, it will get the next element of this buffer.
	glVertexAttribPointer(
		0,					// Attribute 0, no particular reason but same as the vertex shader's layout and glEnableVertexAttribArray
		3,					// Size. Number of values per vertex, must be 1, 2, 3 or 4.
		GL_FLOAT,			// Type of data (GLfloats)
		GL_FALSE,			// Normalized?
		0,					// Stride
		(void*)0			// Array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, mNumberOfVertices); // Draw!
	glDisableVertexAttribArray(0);
}
