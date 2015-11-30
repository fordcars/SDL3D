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

#include <ShadedObject.hpp>
#include <Utils.hpp>

// In:
// - layout location 0: vertex position in modelspace
// - layout location 1: UV coord
// - layout location 2: normal

// Uniforms:
// - mat4 MVP (precalculated)
// - mat4 modelMatrix
// - mat4 viewMatrix
// - mat4 projectionMatrix
// - mat4 normalMatrix
// - sampler2D textureSampler

ShadedObject::ShadedObject(const ObjectGeometry& objectGeometry, ObjectGeometry::constShaderPointer shaderPointer, constTexturePointer texturePointer)
	: BasicObject(objectGeometry, shaderPointer)
{
	mTexturePointer = texturePointer;
}

ShadedObject::~ShadedObject()
{
	// Do nothing
}

void ShadedObject::setTexture(constTexturePointer texturePointer)
{
	mTexturePointer = texturePointer;
}

void ShadedObject::render(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelViewMatrix));

	ObjectGeometry::vec3Buffer& vertexBuffer = getObjectGeometry().getVertexBuffer();
	ObjectGeometry::vec2Buffer& UVBuffer = getObjectGeometry().getUVBuffer();
	ObjectGeometry::vec3Buffer& normalBuffer = getObjectGeometry().getNormalBuffer();

	glUseProgram(getShader()->getID());

	glUniformMatrix4fv(getShader()->findUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(getShader()->findUniform("modelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(getShader()->findUniform("viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	//glUniformMatrix4fv(getShader()->findUniform("projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(getShader()->findUniform("normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniform1i(getShader()->findUniform("textureSampler"), 0); // The first texture, not necessary for now

	// Attribute 0
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

	// Attribute 1
	glEnableVertexAttribArray(1);
	UVBuffer.bind(GL_ARRAY_BUFFER);

	glVertexAttribPointer(
		1,                // Attribute
		2,                // Size (values per vertex)
		GL_FLOAT,         // Type
		GL_FALSE,         // Normalize?
		0,                // Stride
		(void*)0          // Array buffer offset
	);

	// Attribute 2
	glEnableVertexAttribArray(2);
	normalBuffer.bind(GL_ARRAY_BUFFER);

	glVertexAttribPointer(
		2,                // Attribute
		3,                // Size (values per vertex)
		GL_FLOAT,         // Type
		GL_FALSE,         // Normalize?
		0,                // Stride
		(void*)0          // Array buffer offset
	);

	glActiveTexture(GL_TEXTURE0); // Set the active texture unit, you can have more than 1 texture at once
	glBindTexture(GL_TEXTURE_2D, mTexturePointer->getID());
	
	glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.getLength()); // Draw!
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}