#ifndef OBJECT_H_
#define OBJECT_H_

#include <glm/vec3.hpp>
#include <GL/glew.h>
#include <array>

#include <Definitions.h>

typedef std::array<GLfloat, MAX_VERTICES> vertexArray; // Useful to other guys too

class Object
{
private:
	GLuint mVertexBuffer; // Holds vertices
	int mNumberOfVertices;

public:
	Object(vertexArray vertices, const int numberOfVertices);
	~Object();

	void render();
};

#endif /* OBJECT_H_ */