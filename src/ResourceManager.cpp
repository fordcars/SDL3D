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
		
		std::string glLog = getGLShaderDebugLog(shader, glGetShaderiv, glGetShaderInfoLog); // Give it the right functions
		glDeleteProgram(shader);

		info(glLog);
		crash(error);
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

void ResourceManager::appendShader(const std::string &shaderName, GLuint shaderProgram)
{
	glMapPair shader(shaderName, shaderProgram);
	mShaders.insert(shader);
}

void ResourceManager::appendTexture(const std::string &textureName, GLuint texture)
{
	glMapPair texturePair(textureName, texture);
	mTextures.insert(texturePair); // Insert in map
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

std::string ResourceManager::getGLShaderDebugLog(GLuint object, PFNGLGETSHADERIVPROC glGet_iv, PFNGLGETSHADERINFOLOGPROC glGet__InfoLog)
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

	if(vertexShader!=0 && fragmentShader!=0) // Valid shaders
	{
		GLuint shaderProgram = linkShaderProgram(shaderName, vertexShader, fragmentShader);
		appendShader(shaderName, shaderProgram);

		return shaderProgram;
	}

	return 0; // Failed, but error messages should of already been logged by compileShader() and linkShader()
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

// When loading a BMP texture, mipmaps are generated automatically. Consider compressing textures into DDS files and use the corresponding function for adding them.
GLuint ResourceManager::addBMPTexture(const std::string &textureName, const std::string &texturePath) // Adds a texture to the map
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
	if(file == NULL)
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // Change the second GL_RGB for GL_BGR if the colors are inverted

	delete(data);

	// When we stretch (magnify) the image, use linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// When we minify the image, use a linear blend of two mipmaps, each filtered linearly too
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D); // Generate the mipmaps (a bunch of copies of the texture of different sizes)

	appendTexture(textureName, textureID); // Add to the map (keep track of it)

	return textureID; // May be useful?
}

// Loads and adds .DDS textures. Compress using DXT1, DXT3 or DXT5.
GLuint ResourceManager::addDDSTexture(const std::string &textureName, const std::string &texturePath)
{
	unsigned char header[124];

	std::string fullPath = getFullResourcePath(texturePath);
	std::string error = fullPath; // Useful for generating error messages

	FILE *fp;

	// Try to open the file
	fp = fopen(fullPath.c_str(), "rb"); // File pointer
	if(fp == NULL)
	{
		error = "Texture '" + error + "' cannot be opened!";
		crash(error);
		return 0;
	}

	// Verify the type of file
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if(strncmp(filecode, "DDS ", 4) != 0)
	{
		fclose(fp);
		error = "Texture '" + error + "' is not a correct DDS file!";
		crash(error);
		return 0;
	}

	// Get the surface description
	fread(&header, 124, 1, fp);

	unsigned int height        = *(unsigned int*)&(header[8]);
	unsigned int width         = *(unsigned int*)&(header[12]);
	unsigned int linearSize    = *(unsigned int*)&(header[16]);
	unsigned int mipmapCount   = *(unsigned int*)&(header[24]);
	unsigned int fourCC        = *(unsigned int*)&(header[80]);

	unsigned char *buffer;
	unsigned int bufferSize;

	// How big is it going to be, including all mipmaps?
	bufferSize = mipmapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufferSize * sizeof(unsigned char)); // Memory freed before returning
	fread(buffer, 1, bufferSize, fp);

	// Close the file
	fclose(fp);

	// See which format we are dealing with and tell OpenGL what to do with it
	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch(fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;

	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;

	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;

	default:
		free(buffer); // Free the memory!
		error = "Texture '" + error + "' cannot be loaded as DDS file!";
		crash(error);
		return 0;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Fill each mipmap one after another
	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	// Load the mipmaps
	for(unsigned int level = 0; level< mipmapCount && (width || height); ++level)
	{
		unsigned int size = ((width+3)/4) * ((height+3)/4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;
	}

	free(buffer); // Free the memory!

	appendTexture(textureName, textureID);

	return textureID;
}

GLuint ResourceManager::findTexture(const std::string &textureName)
{
	glMap::const_iterator got = mTextures.find(textureName);

	if(got == mTextures.end())
	{
		std::string error = textureName;
		error = "Texture '" + error + "' not found!";
		crash(error);
		return 0;
	}

	return got->second;
}