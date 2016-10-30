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

#include "Shader.hpp"
#include "Utils.hpp"
#include "Definitions.hpp"

#include <limits> // For numeric_limits

// Takes the shader paths for better error logs
Shader::Shader(graphicsManagerPointer graphicsManager, 
				const std::string& name,
				const std::string& vertexShaderPath,
				const std::string& fragmentShaderPath)
{
	mGraphicsManager = graphicsManager;
	mName = name;

	std::string vertexShaderCode = Utils::getFileContents(vertexShaderPath);
	std::string fragmentShaderCode = Utils::getFileContents(fragmentShaderPath);

	GLuint vertexShader = compileShader(vertexShaderPath, vertexShaderCode, GL_VERTEX_SHADER); // Is this length stuff right?
	GLuint fragmentShader = compileShader(fragmentShaderPath, fragmentShaderCode, GL_FRAGMENT_SHADER);

	if(vertexShader!=0 && fragmentShader!=0) // Valid shaders
		mID = linkShaderProgram(mName, vertexShader, fragmentShader);
	else
		mID = 0; // Make sure it doesn't blow up. Error messages should have already been sent.

	registerUniforms(); // Will find all uniforms in the shader and register them
	bindUniformBlocks();
}

Shader::~Shader()
{
	glDeleteShader(mID); // Free memory
}

// PRIVATE

// Static
GLuint Shader::compileShader(const std::string& shaderPath, const std::string& shaderCode, GLenum type) // fileName for debugging
{
	GLuint shader = glCreateShader(type);
	GLint shaderOk;

	std::size_t length = shaderCode.length();

	// But why this weird syntax for a function? Because sometimes Visual Studio is a big pile of urgh.
	// This weird (func)() syntax instead of func() syntax avoids having Visual Studio think max is one of its
	// predefined macros (normally done in windows.h). Normally, you could define NOMINMAX before including a
	// Windows header to solve the problem, but I could not identify where and which Windows header was
	// defining min and max.
	if(length > (std::numeric_limits<unsigned int>::max)() )
	{
		Utils::CRASH("Overflow! Shader at '" + shaderPath  + "' too long! How is this possible?!");
		return 0;
	}

	int shaderLength = static_cast<int>(length);

	const char *shaderFiles[] = {shaderCode.c_str()}; // Pointer to array of pointers
	const int shaderFilesLength[] = {shaderLength}; // Array

	if(shaderLength == 0) // If there is no source
	{
		Utils::CRASH("No shader source found!");
		return 0;
	}
	
	glShaderSource(shader, 1, shaderFiles, shaderFilesLength);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderOk);

	if(!shaderOk)
	{
		std::string error = "Failed to compile shader at '" + shaderPath + "'.";
		
		std::string shaderLog = getGLShaderDebugLog(shader, glGetShaderiv, glGetShaderInfoLog); // Give it the right functions
		glDeleteProgram(shader);

		Utils::LOGPRINT(shaderLog);
		Utils::CRASH(error);
		return 0;
	}

	return shader;
}

// Static
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

		Utils::LOGPRINT(shaderLog);
		Utils::CRASH(error);
		return 0;
	}

	return program;
}

void Shader::registerUniforms()
{
	GLint numberOfUniforms; // Number of uniforms in the shader (linked program)

	// Also counts uniforms inside uniform blocks
	glGetProgramiv(mID, GL_ACTIVE_UNIFORMS, &numberOfUniforms);
	
	const GLsizei bufferSize = 256;
	GLchar uniformNameBuffer[bufferSize]; // Each uniform name will be read to this buffer

	GLsizei numberOfCharsReceived;
	
	// Uniform indices start at 0 and go up to numberOfUniforms
	for(int i=0; i < numberOfUniforms; i++)
	{
		// Check if the uniform is in a uniform block, and ignore it if it is
		GLint index = 0;
		GLuint ui = static_cast<GLuint>(i); // The only safe way to generate a GLuint* from an int
		glGetActiveUniformsiv(mID, 1, &ui, GL_UNIFORM_BLOCK_INDEX, &index);

		if(index == -1) // Uniform not in uniform block (it is in the default, global one)
		{
			glGetActiveUniformName(mID, i, bufferSize, &numberOfCharsReceived, uniformNameBuffer);

			// Buffer is converted to an std::string using the null terminator placed by gl
			registerUniform(uniformNameBuffer);
		}
	}
}

// A uniform is attached to a shader, but can be modified whenever
GLuint Shader::registerUniform(const std::string& uniformName) // Uniform name is the name as in the shader
{
	GLuint uniformLocation = glGetUniformLocation(mID, uniformName.c_str()); // Returns the "index" of the variable in the shader.
	
	if(uniformLocation != -1) // Valid uniform
	{
		gluintMapPair uniformPair(uniformName, uniformLocation);

		std::pair<gluintMap::iterator, bool> newlyAddedPair = mUniformMap.insert(uniformPair); // Insert in map
	
		if(newlyAddedPair.second == false) // Already exists!
		{
			std::string error = "Uniform '" + uniformName + "' in shader '" + mName + "' already exists and cannot be added again!";
			Utils::CRASH(error);
			return newlyAddedPair.first->second; // Returns the uniform that was there before
		}
	} else // Invalid uniform
	{
			std::string error = "Uniform '" + uniformName + "' does not exist or is invalid in shader '" + mName + "'! Are you sure it is active (contributing to the output)?";
			Utils::CRASH(error);
	}

	return uniformLocation; // Return the newly added uniform location
}

// Binds the present uniform blocks to the corresponding buffers
void Shader::bindUniformBlocks()
{
	GLuint index = glGetUniformBlockIndex(mID, GRAPHICS_LIGHT_UNIFORM_BLOCK_NAME);

	if(index != GL_INVALID_INDEX) // The block is defined!
		glUniformBlockBinding(mID, index, GraphicsManager::cLightBindingPoint);
	// Ignore if it isn't defined, not all shaders use lights
}

// PUBLIC

Shader::graphicsManagerPointer Shader::getGraphicsManager() const
{
	return mGraphicsManager;
}

std::string Shader::getName() const
{
	return mName;
}

GLuint Shader::getID() const
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

GLuint Shader::findUniform(const std::string& uniformName) const // Returns a read only int
{
	gluintMap::const_iterator got = mUniformMap.find(uniformName); // Const iterator, we should not need to change this GLuint

	if(got == mUniformMap.end())
	{
		std::string error = uniformName;
		error = "Uniform '" + error + "' was not registered for shader '" + mName + "'! "
			"Is the uniform present in the shader and does it contribute to the shader's output? "
			"Are you creating the correct object type using this shader?";
		Utils::CRASH(error);
		return 0;
	}

	return got->second;
}
