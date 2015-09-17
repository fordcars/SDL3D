#include <Shader.h>
#include <HelperFunctions.h>

#include <ResourceManager.h> // For getFileContents();

using namespace HelperFunctions;

// Takes the shader paths for better error logs
Shader::Shader(const std::string& name,
			   const std::string& vertexShaderPath, const std::string& vertexShaderCode,
			   const std::string& fragmentShaderPath, const std::string& fragmentShaderCode)
	: mName(name) // Keep track of the const name
{
	GLuint vertexShader = compileShader(vertexShaderPath.c_str(), vertexShaderCode.c_str(), GL_VERTEX_SHADER); // Is this length stuff right?
	GLuint fragmentShader = compileShader(fragmentShaderPath.c_str(), fragmentShaderCode.c_str(), GL_FRAGMENT_SHADER);

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
		crash("Overflow! Shader too long! How is this possible?!");
		return 0;
	}

	int shaderLength = static_cast<int>(length);

	const char *shaderFiles[] = {shaderCode.c_str()}; // Pointer to array of pointers
	const int shaderFilesLength[] = {shaderLength}; // Array

	if(shaderLength==0) // If there is no source
		crash("No shader source found!");
	
	glShaderSource(shader, 1, shaderFiles, shaderFilesLength);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderOk);

	if(!shaderOk)
	{
		std::string error = "Failed to compile shader '";
		error = error + shaderPath + "'.";
		
		std::string glLog = getGLShaderDebugLog(shader, glGetShaderiv, glGetShaderInfoLog); // Give it the right functions
		glDeleteProgram(shader);

		info(glLog);
		crash(error);
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
		
		std::string glLog = getGLShaderDebugLog(program, glGetProgramiv, glGetProgramInfoLog); // Give it the right functions
		glDeleteProgram(program);

		info(glLog);
		crash(error);
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
	GLuintMapPair uniformPair(uniformName, uniformLocation);

	std::pair<GLuintMap::iterator, bool> newlyAddedPair = mUniforms.insert(uniformPair); // Insert in map
	
	if(newlyAddedPair.second == false)
	{
		std::string error = "Uniform '" + uniformName + "' in shader '" + mName + "' already exists and cannot be added again!";
		crash(error);
		return newlyAddedPair.first->second; // Returns a reference to the uniform that was there before
	}

	return uniformLocation;
}

void Shader::addUniforms(const std::string uniformNames[], int length)
{
	for(int i=0; i<length; i++)
	{
		addUniform(uniformNames[i]); // Give it the actual value (dereferenced using [i])
	}
}

const GLuint Shader::findUniform(const std::string& uniformName) // Returns a read only int
{
	GLuintMap::const_iterator got = mUniforms.find(uniformName); // Const iterator, we should not need to change this GLuint

	if(got==mUniforms.end())
	{
		std::string error = uniformName;
		error = "Uniform '" + error + "' not found!";
		HelperFunctions::crash(error);
		return 0;
	}

	return got->second;
}