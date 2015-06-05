#include <GL/glew.h>
#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, GLuint> shaderMap;
// A shader is a member of the unordered_map mShaders (std::string shaderName, GLuint shaderProgram)

class ResourceManager
{
private:
	shaderMap mShaders; // Map, faster access: shaders[shaderName] etc
	void appendShader(std::string shaderName, GLuint shaderProgram);

	std::string mResourceDir;

public:
	ResourceManager(const char* resourceDir);
	~ResourceManager();
	static std::string getFileContents(const char *filename);
	static void showGLLog(GLuint object, PFNGLGETSHADERIVPROC glGet_iv, PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);
	static GLuint compileShader(const char *shaderFileName, const char *shaderCode, size_t length, GLenum type);
	static GLuint linkShaderProgram(const char *shaderProgramName, GLuint vertexShader, GLuint fragmentShader);

	GLuint findShader(const char *shaderName);
	void clearShaders();
	void addShader(const char *shaderName, const char *vertexShaderFile, const char *fragmentShaderFile);
};