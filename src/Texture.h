#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <string>
#include <GLAD/glad.h>

// Since I am not feeling like rewriting OpenGL, this class is more of a datatype with functions

class Texture
{
private:
	const std::string mName; // May be useful, for error messages for example
	const int mType;

	GLuint mID;

	static const GLuint getBMPTexture(const std::string& texturePath);
	static const GLuint getDDSTexture(const std::string& texturePath);

public:
	Texture(const std::string& name, const std::string& texturePath, int type);
	~Texture();

	const GLuint getID() const;
	const GLuint getType() const;
};

#endif /* TEXTURE_H_ */