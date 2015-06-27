#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <GL/glew.h>
#include <string>
#include <unordered_map>

// A shader is a member of the unordered_map mShaders (std::string shaderName, GLuint shaderProgram)

class ResourceManager
{
	typedef std::unordered_map<std::string, GLuint> glMap; // string/GLuint map
	typedef std::pair<std::string, GLuint> glMapPair; // typedefs are fun

private:
	glMap mShaders; // Map, faster access: shaders[shaderName] = shaderID etc
	glMap mUniforms; // Uniform variables, uniforms[uniformName] = uniformID

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

	void addShader(const std::string &shaderName, const std::string &vertexShaderFile, const std::string &fragmentShaderFile);
	GLuint findShader(const std::string &shaderName);
	void clearShaders();

	void addUniform(const std::string &uniformName, GLuint shaderProgram);
	void addUniform(const std::string &uniformName, const std::string &shaderName);
	GLuint findUniform(const std::string &uniformName);
};

#endif /* RESOURCEMANAGER_H_ */