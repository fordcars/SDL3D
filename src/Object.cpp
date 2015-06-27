#include <Object.h>

Object::Object(vertexArray vertices, const int numberOfVertices) // Uses std::array for modernism, wasn't necessairy
{
	int vertexDataSize = sizeof(GLfloat) * vertices.size(); // Calculate array size
	GLfloat *vertexData = vertices.data();

	// Done once per object
	glGenBuffers(1, &mVertexBuffer); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer); // Say it's an array
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, GL_STATIC_DRAW); // Give it to OpenGL

	mNumberOfVertices = numberOfVertices;
}

Object::~Object()
{
	// Do nothing
}

void Object::render()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

	glVertexAttribPointer(
		0,					// Attribute 0, no particular reason but I think must be the same as the vertex shader's layout
		3,					// Number of values per vertex, must be 1, 2, 3 or 4.
		GL_FLOAT,			// Type of data (GLfloats)
		GL_FALSE,			// Normalized?
		0,					// Stride
		(void*)0			// Array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, mNumberOfVertices); // Draw!
	glDisableVertexAttribArray(0);
}
