#ifndef TEXTUREDOBJECT_H_
#define TEXTUREDOBJECT_H_

#include <Object.h>
#include <Texture.h>
#include <memory> // For smart pointers

class TexturedObject : public Object // Inherit! 'public' is required here
{
	typedef std::shared_ptr<const Texture> constTexturePointer; // We can't modify the texture

private:
	constTexturePointer mTexture; // Non-const so we can change the texture
	GLuint mUVBuffer;
	const GLuint mTextureTypeUniform;

public:
	TexturedObject(GLfloatArray vertices, int numberOfVertices, GLfloatArray UVCoords, constTexturePointer texture, const GLuint textureTypeUniformLocation);
	~TexturedObject();

	void setTexture(constTexturePointer texture);
	void render(); // Overloading
};

#endif