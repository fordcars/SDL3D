#ifndef TEXTUREDOBJECT_H_
#define TEXTUREDOBJECT_H_

#include <Object.h>

class TexturedObject : public Object // Inherit! 'public' is required here
{
private:
	GLuint mTexture;
	GLuint mUVBuffer;

public:
	TexturedObject(GLfloatArray vertices, int numberOfVertices, GLuint texture, GLfloatArray UVCoords);
	~TexturedObject();

	void render(); // Overloading
};

#endif