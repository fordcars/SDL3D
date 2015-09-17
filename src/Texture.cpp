#include <Texture.h>
#include <Definitions.h> // For various definitions

#include <HelperFunctions.h> // For log

using namespace HelperFunctions;

Texture::Texture(const std::string& name, const std::string& texturePath, int type)
	: mName(name), mType(type)
{
	switch(type)
	{
	case BMP_TEXTURE:
		mID = getBMPTexture(texturePath);
		break;

	case DDS_TEXTURE:
		mID = getDDSTexture(texturePath);
		break;

	default:
		break;
	}
}

Texture::~Texture()
{
	glDeleteTextures(1, &mID); // Delete this texture. Might save memory.
}

// When loading a BMP texture, mipmaps are generated automatically. Consider compressing textures into DDS files and use the corresponding function for adding them.
const GLuint Texture::getBMPTexture(const std::string& texturePath) // Adds a texture to the map
{
	// Not the best code for getting BMP data
	unsigned char header[54];
	unsigned int dataPos;
	unsigned width, height;
	unsigned int imageSize;
	unsigned char *data; // Actual RGB data

	std::string error = texturePath; // Use this for error messages

	FILE *file = fopen(texturePath.c_str(), "rb");
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

	return textureID; // Return the id
}

// Loads and adds .DDS textures. Compress using DXT1, DXT3 or DXT5.
const GLuint Texture::getDDSTexture(const std::string& texturePath)
{
	unsigned char header[124];
	std::string error = texturePath; // Useful for generating error messages

	FILE *fp;

	// Try to open the file
	fp = fopen(texturePath.c_str(), "rb"); // File pointer
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

	return textureID;
}

const GLuint Texture::getID() const // Can be called from const instances
{
	return mID;
}

const GLuint Texture::getType() const
{
	return mType;
}