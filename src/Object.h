#ifndef OBJECT_H_
#define OBJECT_H_

#include <GLAD/glad.h>
#include <glm/glm.hpp>
#include <memory>

#include <Shader.h>
#include <Definitions.h>

// This is the base class of 3d objects. This class holds the vertices.
// This class can also live on it's own.
// Eventually, we could have 1 vertex buffer holding all of the objects.

class Object
{
	typedef std::shared_ptr<const Shader> shaderPointer;

private:
	GLuint mVertexBuffer; // Holds vertices
	int mNumberOfVertices;

	shaderPointer mShader; // The shader used to render this object

protected: // Only accessible to derived classes
	GLuint getVertexBuffer() const { return mVertexBuffer; } // Useful for render functions, these are read only!
	int getNumberOfVertices() const { return mNumberOfVertices; } // Useful for render functions, these are read only!

public:
	Object(GLfloatArray vertices, int numberOfVertices, shaderPointer shader);
	~Object();

	void setShader(shaderPointer shader);

	virtual void render(); // Overload this if you need to!
};

#endif /* OBJECT_H_ */