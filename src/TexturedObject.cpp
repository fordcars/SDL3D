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

#include <TexturedObject.hpp>
#include <HelperFunctions.hpp>

// Uniforms:
// - mat4 MVP
// - sampler2D textureSampler
// - int textureType

// In:
// - vertex position in modelspace
// - UV coords

TexturedObject::TexturedObject(GLfloatVector vertices, GLfloatVector UVCoords, shaderPointer shader,
							   constTexturePointer texturePointer)
	: Object(vertices, shader) // Calls Object constructor with those arguments
{
	mTexturePointer = texturePointer;

	mUVBuffer.bind(GL_ARRAY_BUFFER);
	mUVBuffer.setMutableData(GL_ARRAY_BUFFER, UVCoords, GL_DYNAMIC_DRAW);
}

TexturedObject::~TexturedObject()
{
	// Do nothing
}

TexturedObject::GLfloatBuffer &TexturedObject::getUVBuffer()
{
	return mUVBuffer;
}

void TexturedObject::setTexture(constTexturePointer texturePointer)
{
	mTexturePointer = texturePointer;
}

void TexturedObject::render(glm::mat4 MVP)
{
	glUseProgram(getShader()->getID());

	glUniformMatrix4fv(getShader()->findUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniform1i(getShader()->findUniform("textureSampler"), 0); // The first texture, not necessary for now
	glUniform1i(getShader()->findUniform("textureType"), mTexturePointer->getType()); // Send the type over to the shader

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, getVertexBuffer()); // All future function calls will modify this vertex buffer

	glVertexAttribPointer(
		0,					// Attribute 0, no particular reason but same as the vertex shader's layout and glEnableVertexAttribArray
		3,					// Size. Number of values per vertex, must be 1, 2, 3 or 4.
		GL_FLOAT,			// Type of data (GLfloats)
		GL_FALSE,			// Normalized?
		0,					// Stride
		(void*)0			// Array buffer offset
	);	

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer); // All future function calls will modify this vertex buffer

	glVertexAttribPointer(
		1,					// Attribute 1, no particular reason but same as the vertex shader's layout and glEnableVertexAttribArray
		2,					// Size. Number of values per cell, must be 1, 2, 3 or 4.
		GL_FLOAT,			// Type of data (GLfloats)
		GL_FALSE,			// Normalized?
		0,					// Stride
		(void*)0			// Array buffer offset
	);

	glActiveTexture(GL_TEXTURE0); // Set the active texture unit, you can have more than 1 texture at once
	glBindTexture(GL_TEXTURE_2D, mTexturePointer->getID());

	glDrawArrays(GL_TRIANGLES, 0, getNumberOfVertices()); // Draw!

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}