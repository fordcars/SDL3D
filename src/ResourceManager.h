#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <GL/glew.h>
#include <string>
#include <unordered_map>

// A shader is a member of the unordered_map mShaders (std::string shaderName, GLuint shaderProgram)
// All paths are prefixed with mResourceDir

class ResourceManager
{
	typedef std::unordered_map<std::string, GLuint> glMap; // string/GLuint map, extremely useful iaminlovewithopengl
	typedef std::pair<std::string, GLuint> glMapPair; // typedefs are fun

private:
	glMap mShaders; // Map, faster access: shaders[shaderName] = shaderID etc
	glMap mUniforms; // Uniform variables, uniforms[uniformName] = uniformID
	glMap mTextures;

	// Static because they donnot need an instance to work
	static GLuint compileShader(const std::string &shaderFileName, const std::string &shaderCode, size_t length, GLenum type);
	static GLuint linkShaderProgram(const std::string &shaderProgramName, GLuint vertexShader, GLuint fragmentShader);

	void appendShader(const std::string &shaderName, GLuint shaderProgram);

	std::string mResourceDir;

public:
	ResourceManager(const std::string &resourceDir);
	~ResourceManager();

	static std::string getFileContents(const std::string &fileName); // Static functions: no need for an instance to use them!
	static void showGLLog(GLuint object, PFNGLGETSHADERIVPROC glGet_iv, PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);

	std::string getFullResourcePath(const std::string &fileName);

	GLuint addShader(const std::string &shaderName, const std::string &vertexShaderFile, const std::string &fragmentShaderFile);
	GLuint findShader(const std::string &shaderName);
	void clearShaders();

	GLuint addUniform(const std::string &uniformName, GLuint shaderProgram);
	GLuint addUniform(const std::string &uniformName, const std::string &shaderName);
	GLuint findUniform(const std::string &uniformName);

	GLuint addTexture(const std::string &textureName, const std::string &texturePath);
	GLuint findTexture(const std::string &textureName);
};

#endif /* RESOURCEMANAGER_H_ */