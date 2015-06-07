#include <ResourceManager.h>
#include <fstream>
#include <HelperFunctions.h>

// Shaders must be ASCII
// Reference: http://duriansoftware.com/joe/An-intro-to-modern-OpenGL.-Chapter-2.2:-Shaders.html

using namespace HelperFunctions;

ResourceManager::ResourceManager(const std::string &resourceDir)
{
	mResourceDir = resourceDir;
}

ResourceManager::~ResourceManager()
{
	clearShaders();
}

void ResourceManager::appendShader(const std::string &shaderName, GLuint shaderProgram)
{
	std::pair<std::string, GLuint> shader (shaderName, shaderProgram);
	mShaders.insert(shader);
}

std::string ResourceManager::getFileContents(const std::string &filePath) // Returns the contents of the file
{
	std::ifstream in(filePath, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;

		in.seekg(0, std::ios::end);
		contents.resize((int)in.tellg()); // ASCII?
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	} else
	{
		std::string crashLog = filePath;
		crashLog += " cannot be opened!";
		crash(crashLog);
		return 0;
	}
}

void ResourceManager::showGLLog(GLuint object, PFNGLGETSHADERIVPROC glGet_iv, PFNGLGETSHADERINFOLOGPROC glGet__InfoLog)
{
	GLint logLength;
	char *log;
	
	glGet_iv(object, GL_INFO_LOG_LENGTH, &logLength);
	log = (char *)malloc(logLength);

	glGet__InfoLog(object, logLength, NULL, log);
	info(log);
	free(log);
}


GLuint ResourceManager::compileShader(const std::string &shaderFileName, const std::string &shaderCode, size_t length, GLenum type) // fileName for debugging
{
	GLuint shader = glCreateShader(type);
	GLint shaderOk;

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
		error = error + shaderFileName + "'.";
		crash(error);
		showGLLog(shader, glGetShaderiv, glGetShaderInfoLog); // More log info
		glDeleteShader(shader);

		return 0;
	}

	return shader;
}

GLuint ResourceManager::linkShaderProgram(const std::string &shaderProgramName, GLuint vertexShader, GLuint fragmentShader)
{
	GLint programOk;

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &programOk);

	if(!programOk)
	{
		std::string error = "Failed to link shader program '";
		error = error + shaderProgramName + "'.";
		crash(error);
		showGLLog(program, glGetProgramiv, glGetProgramInfoLog); // More log info
		glDeleteProgram(program);

		return 0;
	}
	
	return program;
}

GLuint ResourceManager::findShader(const std::string &shaderName)
{
	// http://www.cplusplus.com/reference/unordered_map/unordered_map/find/
	shaderMap::const_iterator got = mShaders.find(shaderName);

	if(got==mShaders.end())
	{
		std::string error = shaderName;
		error = "Shader '" + error + "' not found!";
		crash(error);
		return 0;
	}

	return got->second; // Dunno why you need ->
}

void ResourceManager::clearShaders()
{
	mShaders.clear(); // Clears all shaders (if you want to know, calls all deconstructors)
}

void ResourceManager::addShader(const std::string &shaderName, const std::string &vertexShaderFile, const std::string &fragmentShaderFile)
{
	std::string vertexFilePath = vertexShaderFile; // Copy strings, is this useful?
	std::string fragmentFilePath = fragmentShaderFile;

	vertexFilePath = mResourceDir + vertexFilePath; // All resources are in the resource dir
	fragmentFilePath = mResourceDir + fragmentFilePath;

	std::string vertexShaderCode = getFileContents(vertexFilePath.c_str());
	std::string fragmentShaderCode = getFileContents(fragmentFilePath.c_str());

	GLuint vertexShader = compileShader(vertexFilePath.c_str(), vertexShaderCode.c_str(), vertexShaderCode.length(), GL_VERTEX_SHADER); // Is this length stuff right?
	GLuint fragmentShader = compileShader(fragmentFilePath.c_str(), fragmentShaderCode.c_str(), fragmentShaderCode.length(), GL_FRAGMENT_SHADER);
	GLuint shaderProgram = linkShaderProgram(shaderName, vertexShader, fragmentShader);

	appendShader(shaderName, shaderProgram);
}
