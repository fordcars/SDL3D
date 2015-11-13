//// Copyright 2015 Carl Hewett
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

#include <Shader.hpp>
#include <Utils.hpp>

#include <ResourceManager.hpp> // For getFileContents();

// Takes the shader paths for better error logs
Shader::Shader(const std::string& name,
			   const std::string& vertexShaderPath,
			   const std::string& fragmentShaderPath)
{
	mName = name;

	std::string vertexShaderCode = Utils::getFileContents(vertexShaderPath);
	std::string fragmentShaderCode = Utils::getFileContents(fragmentShaderPath);

	GLuint vertexShader = compileShader(vertexShaderPath, vertexShaderCode, GL_VERTEX_SHADER); // Is this length stuff right?
	GLuint fragmentShader = compileShader(fragmentShaderPath, fragmentShaderCode, GL_FRAGMENT_SHADER);

	if(vertexShader!=0 && fragmentShader!=0) // Valid shaders
		mID = linkShaderProgram(mName, vertexShader, fragmentShader);
	else
		mID = 0; // Make sure it doesn't blow up. Error messages should have already been sent.
}

Shader::~Shader()
{
	glDeleteShader(mID); // Free memory
}

GLuint Shader::compileShader(const std::string& shaderPath, const std::string& shaderCode, GLenum type) // fileName for debugging
{
	GLuint shader = glCreateShader(type);
	GLint shaderOk;

	size_t length = shaderCode.length();

	if(length>INT_MAX)
	{
		Utils::crash("Overflow! Shader too long! How is this possible?!", __LINE__, __FILE__);
		return 0;
	}

	int shaderLength = static_cast<int>(length);

	const char *shaderFiles[] = {shaderCode.c_str()}; // Pointer to array of pointers
	const int shaderFilesLength[] = {shaderLength}; // Array

	if(shaderLength==0) // If there is no source
		Utils::crash("No shader source found!", __LINE__, __FILE__);
	
	glShaderSource(shader, 1, shaderFiles, shaderFilesLength);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderOk);

	if(!shaderOk)
	{
		std::string error = "Failed to compile shader '";
		error = error + shaderPath + "'.";
		
		std::string shaderLog = getGLShaderDebugLog(shader, glGetShaderiv, glGetShaderInfoLog); // Give it the right functions
		glDeleteProgram(shader);

		Utils::logprint(shaderLog);
		Utils::crash(error, __LINE__, __FILE__);
		return 0;
	}

	return shader;
}

GLuint Shader::linkShaderProgram(const std::string& shaderProgramName, GLuint vertexShader, GLuint fragmentShader)
{
	GLint programOk;

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &programOk);

	if(!programOk)
	{
		std::string error = "Failed to link shader program'";
		error = error + shaderProgramName + "'.";
		
		std::string shaderLog = getGLShaderDebugLog(program, glGetProgramiv, glGetProgramInfoLog); // Give it the right functions
		glDeleteProgram(program);

		Utils::logprint(shaderLog);
		Utils::crash(error, __LINE__, __FILE__);
		return 0;
	}
	
	return program;
}

const GLuint Shader::getID() const
{
	return mID;
}

std::string Shader::getGLShaderDebugLog(GLuint object, PFNGLGETSHADERIVPROC glGet_iv, PFNGLGETSHADERINFOLOGPROC glGet__InfoLog)
{
	GLint logLength; // Amount of characters
	std::string log;
	
	glGet_iv(object, GL_INFO_LOG_LENGTH, &logLength); // Get size
	log.resize(logLength); // Resize string

	if(logLength)
		glGet__InfoLog(object, logLength, NULL, &log[0]);

	log.pop_back(); // Remove null terminator (\0) that OpenGL added
	return "\n-----------GL LOG-----------\n" + log; // For looks
}

// A uniform is attached to a shader, but can be modified whenever
const GLuint Shader::addUniform(const std::string& uniformName) // Uniform name is the name as in the shader
{
	GLuint uniformLocation = glGetUniformLocation(mID, uniformName.c_str()); // Returns the "index" of the variable in the shader.
	
	if(uniformLocation != -1) // Valid uniform
	{
		GLuintMapPair uniformPair(uniformName, uniformLocation);

		std::pair<GLuintMap::iterator, bool> newlyAddedPair = mUniformMap.insert(uniformPair); // Insert in map
	
		if(newlyAddedPair.second == false) // Already exists!
		{
			std::string error = "Uniform '" + uniformName + "' in shader '" + mName + "' already exists and cannot be added again!";
			Utils::crash(error, __LINE__, __FILE__);
			return newlyAddedPair.first->second; // Returns the uniform that was there before
		}
	} else // Invalid uniform
	{
			std::string error = "Uniform '" + uniformName + "' does not exist or is invalid in shader '" + mName + "'!";
			Utils::crash(error, __LINE__, __FILE__);
	}

	return uniformLocation; // Return the newly added uniform location
}

void Shader::addUniforms(const std::string uniformNames[], int length)
{
	for(int i=0; i<length; i++)
	{
		addUniform(uniformNames[i]); // Give it the actual value (dereferenced using [i])
	}
}

const GLuint Shader::findUniform(const std::string& uniformName) const // Returns a read only int
{
	GLuintMap::const_iterator got = mUniformMap.find(uniformName); // Const iterator, we should not need to change this GLuint

	if(got==mUniformMap.end())
	{
		std::string error = uniformName;
		error = "Uniform '" + error + "' not found!";
		Utils::crash(error, __LINE__, __FILE__);
		return 0;
	}

	return got->second;
}