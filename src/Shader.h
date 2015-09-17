#ifndef SHADER_H_
#define SHADER_H_

#include <unordered_map>
#include <GLAD/glad.h>
#include <string>

#include <Definitions.h>

class Shader
{
private:
	GLuint mID; // the ID of the shader, give this to OpenGL stuff. Could be const, but I left it non-const to make things easier.
	GLuintMap mUniforms; // Uniform variables, uniforms[uniformName] = uniform location

	const std::string mName; // Useful for error messages

	// Static because they donnot need an instance to work
	static GLuint compileShader(const std::string& shaderPath, const std::string& shaderCode, GLenum type);
	static GLuint linkShaderProgram(const std::string& shaderProgramName, GLuint vertexShader, GLuint fragmentShader);

public:
	Shader(const std::string& name, const std::string& vertexShaderPath, const std::string& vertexShaderCode, const std::string &fragmentShaderPath, const std::string &fragmentShaderCode);
	~Shader();

	const GLuint getID() const;
	static std::string getGLShaderDebugLog(GLuint object, PFNGLGETSHADERIVPROC glGet_iv, PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);

	const GLuint addUniform(const std::string& uniformName);
	void addUniforms(const std::string uniformNames[], int length); // Const variable, non-const pointer
	const GLuint findUniform(const std::string& uniformName);
};

#endif