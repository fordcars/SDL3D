#include <TexturedObject.h>
#include <HelperFunctions.h>

TexturedObject::TexturedObject(GLfloatArray vertices, int numberOfVertices, GLfloatArray UVCoords,
							   constTexturePointer texture, const GLuint textureTypeIntUniform)
	: Object(vertices, numberOfVertices), // Calls Object constructor with those arguments
	mTextureTypeUniform(textureTypeIntUniform)
{
	mTexture = texture;

	int UVArraySize = sizeof(GLfloat) * numberOfVertices * 2; // Calculate array size
	GLfloat *UVData = UVCoords.data();

	// Create a VBO. Done once per object
	glGenBuffers(1, &mUVBuffer); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer); // Say it's an array

	// (GL_STATIC_DRAW means that the data will only be modified once)
	glBufferData(GL_ARRAY_BUFFER, UVArraySize, UVData, GL_STATIC_DRAW); // Give it to OpenGL
}

TexturedObject::~TexturedObject()
{
	// Do nothing
}

void TexturedObject::setTexture(constTexturePointer texture)
{
	mTexture = texture;
}

void TexturedObject::render()
{
	glUniform1i(mTextureTypeUniform, mTexture->getType()); // Send the type over to the shader

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
	glBindTexture(GL_TEXTURE_2D, mTexture->getID());

	glDrawArrays(GL_TRIANGLES, 0, getNumberOfVertices()); // Draw!

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}