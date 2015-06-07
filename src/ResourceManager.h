#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <GL/glew.h>
#include <string>
#include <unordered_map>

// A shader is a member of the unordered_map mShaders (std::string shaderName, GLuint shaderProgram)

class ResourceManager
{
	typedef std::unordered_map<std::string, GLuint> shaderMap;

private:
	
	void appendShader(const std::string &shaderName, GLuint shaderProgram);

	std::string mResourceDir;

public:
shaderMap mShaders; // Map, faster access: shaders[shaderName] etc
	ResourceManager(const std::string &resourceDir);
	~ResourceManager();
	static std::string getFileContents(const std::string &fileName);
	static void showGLLog(GLuint object, PFNGLGETSHADERIVPROC glGet_iv, PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);
	static GLuint compileShader(const std::string &shaderFileName, const std::string &shaderCode, size_t length, GLenum type);
	static GLuint linkShaderProgram(const std::string &shaderProgramName, GLuint vertexShader, GLuint fragmentShader);

	GLuint findShader(const std::string &shaderName);
	void clearShaders();
	void addShader(const std::string &shaderName, const std::string &vertexShaderFile, const std::string &fragmentShaderFile);
};

#endif /* RESOURCEMANAGER_H_ */