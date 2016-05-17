//// Copyright 2016 Carl Hewett
////
//// This file is part of SDL3D.
////
//// SDL3D is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// SDL3D is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifndef SHADER_HPP
#define SHADER_HPP

#include <map>
#include <glad/glad.h>
#include <string>

class Shader
{
private:
	using GLuintMap = std::map<std::string, GLuint>;
	using GLuintMapPair = std::pair<std::string, GLuint>;

	std::string mName; // Useful for error messages, don't change this stupidly

	GLuint mID; // the ID of the shader, give this to OpenGL stuff. Could be const, but I left it non-const to make things easier.
	GLuintMap mUniformMap; // Uniform variables, uniforms[uniformName] = uniform location

	// Static because they donnot need an instance to work
	static GLuint compileShader(const std::string& shaderPath, const std::string& shaderCode, GLenum type);
	static GLuint linkShaderProgram(const std::string& shaderProgramName, GLuint vertexShader, GLuint fragmentShader);

	void registerUniforms();
	GLuint registerUniform(const std::string& uniformName);

public:
	Shader(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~Shader();

	std::string getName() const;
	GLuint getID() const;
	static std::string getGLShaderDebugLog(GLuint object, PFNGLGETSHADERIVPROC glGet_iv, PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);
	GLuint findUniform(const std::string& uniformName) const;
};

#endif // SHADER_HPP