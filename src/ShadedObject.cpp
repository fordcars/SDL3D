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

#include "ShadedObject.hpp"
#include "ObjectGeometry.hpp"
#include "Camera.hpp"
#include "Utils.hpp"

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
// - int lightCount

// Uniform blocks:
// Lights:
/*
#define MAX_LIGHTS 1000

uniform numberOfLights;

struct Light
{
vec3 position;
vec3 diffuseColor;
vec3 specularColor;
float power;
float isOn; // 1.0 if the light is on, 0.0 if it is off
}

layout(std140) uniform Lights
{
Light lights[MAX_NUMBER_OF_LIGHTS];
};
*/

ShadedObject::ShadedObject(constObjectGeometryPointer objectGeometry,
						   constShaderPointer shaderPointer, constTexturePointer texturePointer,
						   bool physicsCircularShape, int physicsType)
	: TexturedObject(objectGeometry, shaderPointer, texturePointer, physicsCircularShape, physicsType)
{
	// Do nothing
}

ShadedObject::~ShadedObject()
{
	// Do nothing
}

void ShadedObject::render(const Camera& camera)
{
	const ObjectGeometry::uintBuffer& indexBuffer = getObjectGeometry()->getIndexBuffer();
	const ObjectGeometry::vec3Buffer& positionBuffer = getObjectGeometry()->getPositionBuffer();
	const ObjectGeometry::vec2Buffer& UVBuffer = getObjectGeometry()->getUVBuffer();
	const ObjectGeometry::vec3Buffer& normalBuffer = getObjectGeometry()->getNormalBuffer();

	glm::mat4 modelMatrix = getPhysicsBody().generateModelMatrix();
	glm::mat4 viewMatrix = camera.getViewMatrix();
	glm::mat4 projectionMatrix = camera.getProjectionMatrix();

	glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelViewMatrix));

	glUseProgram(getShader()->getID());

	glUniformMatrix4fv(getShader()->findUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(getShader()->findUniform("modelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(getShader()->findUniform("viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	//glUniformMatrix4fv(getShader()->findUniform("projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(getShader()->findUniform("normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	glUniform1i(getShader()->findUniform("textureSampler"), 0); // The first texture, 0
	glUniform1i(getShader()->findUniform("lightCount"), getShader()->getGraphicsManager()->getLightCount());

	// Attribute 0, position buffer
	glEnableVertexAttribArray(0);
	positionBuffer.bind(GL_ARRAY_BUFFER);
	glVertexAttribPointer(
		0,					// Attribute 0, no particular reason but same as the vertex shader's layout and glEnableVertexAttribArray
		3,					// Size. Number of values per vertex, must be 1, 2, 3 or 4.
		GL_FLOAT,			// Type of data (GLfloats)
		GL_FALSE,			// Normalized?
		0,					// Stride
		(void*)0			// Array buffer offset
	);

	// Attribute 1, UV buffer
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

	// Attribute 2, normal buffer
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

	// Texture
	glActiveTexture(GL_TEXTURE0); // Set the active texture unit, you can have more than 1 texture at once
	glBindTexture(GL_TEXTURE_2D, getTexture()->getID());

	// Draw!
	// Use the index buffer, more efficient!
	glDrawElements(
		GL_TRIANGLES,            // Mode
		indexBuffer.getLength(), // Count
		GL_UNSIGNED_INT,         // Type
		(void*)0                 // Element array buffer offset
	);

	// Disable vertex attrib arrays
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
