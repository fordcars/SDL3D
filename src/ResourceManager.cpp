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

void ResourceManager::appendShader(const std::string &shaderName, GLuint shaderProgram)
{
	glMapPair shader(shaderName, shaderProgram);
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

// Returns the full resource oath
std::string ResourceManager::getFullResourcePath(const std::string &resourcePath)
{
	return mResourceDir + resourcePath;
}

GLuint ResourceManager::addShader(const std::string &shaderName, const std::string &vertexShaderFile, const std::string &fragmentShaderFile)
{
	std::string vertexFilePath = vertexShaderFile; // Copy strings, is this useful?
	std::string fragmentFilePath = fragmentShaderFile;

	vertexFilePath = getFullResourcePath(vertexFilePath); // All resources are in the resource dir
	fragmentFilePath = getFullResourcePath(fragmentFilePath);

	std::string vertexShaderCode = getFileContents(vertexFilePath.c_str());
	std::string fragmentShaderCode = getFileContents(fragmentFilePath.c_str());

	GLuint vertexShader = compileShader(vertexFilePath.c_str(), vertexShaderCode.c_str(), vertexShaderCode.length(), GL_VERTEX_SHADER); // Is this length stuff right?
	GLuint fragmentShader = compileShader(fragmentFilePath.c_str(), fragmentShaderCode.c_str(), fragmentShaderCode.length(), GL_FRAGMENT_SHADER);
	GLuint shaderProgram = linkShaderProgram(shaderName, vertexShader, fragmentShader);

	appendShader(shaderName, shaderProgram);

	return shaderProgram;
}

GLuint ResourceManager::findShader(const std::string &shaderName)
{
	// http://www.cplusplus.com/reference/unordered_map/unordered_map/find/
	glMap::const_iterator got = mShaders.find(shaderName);

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

// A uniform is attached to a shader, but can be modified whenever
GLuint ResourceManager::addUniform(const std::string &uniformName, GLuint shader) // Uniform name is the name as in the shader
{
	GLuint uniformID = glGetUniformLocation(shader, uniformName.c_str()); // Create the uniform variable
	glMapPair uniform(uniformName, uniformID);
	mUniforms.insert(uniform); // Insert in map

	return uniformID;
}

GLuint ResourceManager::addUniform(const std::string &uniformName, const std::string &shaderName) // For us lazy developers
{
	GLuint shader = findShader(shaderName);
	return addUniform(uniformName, shader);
}

GLuint ResourceManager::findUniform(const std::string &uniformName)
{
	glMap::const_iterator got = mUniforms.find(uniformName);

	if(got==mUniforms.end())
	{
		std::string error = uniformName;
		error = "Uniform '" + error + "' not found!";
		crash(error);
		return 0;
	}

	return got->second;
}

GLuint ResourceManager::addTexture(const std::string &textureName, const std::string &texturePath) // Adds a texture to the map
{
	// Not the best code for getting BMP data
	unsigned char header[54];
	unsigned int dataPos;
	unsigned width, height;
	unsigned int imageSize;
	unsigned char *data; // Actual RGB data

	std::string fullPath = getFullResourcePath(texturePath);

	std::string error = fullPath; // Use this for error messages

	FILE *file = fopen(fullPath.c_str(), "rb");
	if(!file)
	{
		error = "BMP image '" + error + "' could not be opened!";
		crash(error);
		return 0;
	}

	if(fread(header, 1, 54, file) != 54) // If it's not 54 bytes, crash!
	{
		error = "BMP image '" + error + "' is not a correct BMP file! (Header is not 54 bytes)";
		crash(error);
		return 0;
	}

	if(header[0] != 'B' || header[1] != 'M') // Not BMP file?
	{
		error = "BMP image '" + error + "' is not a correct BMP file! (No 'BM' present in header)";
		crash(error);
		return 0;
	}

	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files suck and miss some info, lets find those out if they are
	if(imageSize==0)	imageSize = width*height*3; // 3: BGR I guess
	if(dataPos==0)	    dataPos = 54; // The header is done this way

	// Create a buffer
	data = new unsigned char [imageSize]; // This gets deleted after glTexImage2D()

	// Read the actual data
	fread(data, 1, imageSize, file);

	// Everything is in memory now, close the file
	fclose(file);

	// OpenGL
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the new texture so that future functions will modify this
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	delete(data);

	// When we stretch (magnify) the image, use linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// When we minify the image, use a linear blend of two mipmaps, each filtered linearly too
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D); // Generate the mipmaps (a bunch of copies of the texture of different sizes)

	// Add the texture to the map
	glMapPair texturePair(textureName, textureID);
	mTextures.insert(texturePair); // Insert in map

	return textureID;
}

GLuint ResourceManager::findTexture(const std::string &textureName)
{
	glMap::const_iterator got = mTextures.find(textureName);

	if(got==mTextures.end())
	{
		std::string error = textureName;
		error = "Texture '" + error + "' not found!";
		crash(error);
		return 0;
	}

	return got->second;
}