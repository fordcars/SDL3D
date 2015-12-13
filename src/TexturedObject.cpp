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

#include <TexturedObject.hpp>
#include <Utils.hpp>

// In:
// - layout location 0: vertex position in modelspace
// - layout location 1: UV coord

// Uniforms:
// - mat4 MVP
// - sampler2D textureSampler

TexturedObject::TexturedObject(const ObjectGeometry& objectGeometry, ObjectGeometry::constShaderPointer shaderPointer, constTexturePointer texturePointer)
	: Object(objectGeometry, shaderPointer) // Calls Object constructor with those arguments
{
	mTexturePointer = texturePointer;
}

TexturedObject::~TexturedObject()
{
	// Do nothing
}

void TexturedObject::setTexture(constTexturePointer texturePointer)
{
	mTexturePointer = texturePointer;
}

void TexturedObject::render(const Camera& camera)
{
	glm::mat4 MVP = camera.getProjectionMatrix() * camera.getViewMatrix() * getModelMatrix();

	ObjectGeometry::vec3Buffer& vertexBuffer = getObjectGeometry().getVertexBuffer();
	ObjectGeometry::vec2Buffer& UVBuffer = getObjectGeometry().getUVBuffer();
	
	glUseProgram(getShader()->getID());

	glUniformMatrix4fv(getShader()->findUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniform1i(getShader()->findUniform("textureSampler"), 0); // The first texture, not necessary for now

	glEnableVertexAttribArray(0);
	vertexBuffer.bind(GL_ARRAY_BUFFER);
	
	glVertexAttribPointer(
		0,					// Attribute 0, no particular reason but same as the vertex shader's layout and glEnableVertexAttribArray
		3,					// Size. Number of values per vertex, must be 1, 2, 3 or 4.
		GL_FLOAT,			// Type of data (GLfloats)
		GL_FALSE,			// Normalized?
		0,					// Stride
		(void*)0			// Array buffer offset
	);

	glEnableVertexAttribArray(1);
	UVBuffer.bind(GL_ARRAY_BUFFER);

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
	
	glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.getLength()); // Draw!
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}