#ifndef OBJECT_H_
#define OBJECT_H_

#include <glm/vec3.hpp>
#include <GL/glew.h>
#include <array>

#include <Definitions.h>

// This is the base class of 3d objects. This class holds the vertices.
// This class can also live on it's own.
// Eventually, we could have 1 vertex buffer holding all of the objects.

typedef std::array<GLfloat, MAX_GL_ARRAY_LENGTH> GLfloatArray; // Useful to other guys too.

class Object
{
private:
	GLuint mVertexBuffer; // Holds vertices
	int mNumberOfVertices;

protected: // Only accessible to derived classes
	GLuint getVertexBuffer() const { return mVertexBuffer; } // Useful for render functions, these are read only!
	int getNumberOfVertices() const { return mNumberOfVertices; } // Useful for render functions, these are read only!

public:
	Object(GLfloatArray vertices, int numberOfVertices);
	~Object();

	virtual void render(); // Overload this if you need to!
};

#endif /* OBJECT_H_ */