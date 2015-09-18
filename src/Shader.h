// Copyright 2015 Carl Hewett

// This file is part of SDL3D.

// SDL3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// SDL3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.

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
	const GLuint findUniform(const std::string& uniformName) const;
};

#endif /* SHADER_H_ */