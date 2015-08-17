#include <Object.h>
#include <HelperFunctions.h>

Object::Object(GLfloatArray vertices, int numberOfVertices) // Uses std::array for modernism, wasn't necessairy
{
	mNumberOfVertices = numberOfVertices;
	
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

void Object::render()
{
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
