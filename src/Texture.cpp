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

#include <Texture.hpp>
#include <Definitions.hpp> // For various definitions

#include <Utils.hpp> // For log

#include <fstream> // For files
#include <vector>
#include <cstring> // For strncmp

Texture::Texture(const std::string& name, const std::string& texturePath, int type)
{
	mName = name;

	switch(type)
	{
	case BMP_TEXTURE:
		mID = loadBMPTexture(texturePath);
		break;

	case DDS_TEXTURE:
		mID = loadDDSTexture(texturePath);
		break;

	default:
		std::string warning = "No texture type specified for textue '" + name + "'! Assuming it is bipmap";
		Utils::WARN(warning);
		break;
	}
}

Texture::~Texture()
{
	glDeleteTextures(1, &mID); // Delete this texture. Might save memory.
}

// Static
// When loading a BMP texture, mipmaps are generated automatically. Consider compressing textures into DDS files and use the corresponding function for adding them.
GLuint Texture::loadBMPTexture(const std::string& texturePath) // Adds a texture to the map
{
	// Not the best code for getting BMP data
	const int headerSize = 54;
	std::vector<char> header(headerSize);

	unsigned int dataPos;
	unsigned width, height;
	unsigned int imageSize;
	std::vector<char> pixelData; // The actual pixel data

	std::ifstream file(texturePath, std::ios::binary);

	if(!file)
	{
		std::string error = "BMP image '" + texturePath + "' could not be opened!";
		Utils::CRASH(error);
		return 0;
	}

	file.read(header.data(), headerSize); // Give the address of the first element, and read() makes a pointer to it (internally)

	if(file.gcount() != 54) // If it's not 54 bytes, crash!
	{
		std::string error = "BMP image '" + texturePath + "' is not a correct BMP file! (Header is not 54 bytes)";
		Utils::CRASH(error);
		file.close();

		return 0;
	}

	if(header[0] != 'B' || header[1] != 'M') // Not BMP file?
	{
		std::string error = "BMP image '" + texturePath + "' is not a correct BMP file! (No 'BM' present in header)";
		Utils::CRASH(error);
		file.close();

		return 0;
	}

	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files suck and miss some info, lets find those out if they are
	if(imageSize==0)	imageSize = width*height*3; // 3: RGB I guess
	if(dataPos==0)	    dataPos = 54; // The header is done this way

	// Create a buffer
	pixelData.resize(imageSize);

	// Read the actual data
	file.read(pixelData.data(), imageSize);

	// Everything is in memory now, close the file
	file.close();

	// OpenGL
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the new texture so that future functions will modify this
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	// The second color format (GL_RGB or GL_BGR) can be changed to invert colors
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, pixelData.data());

	// Filtering
	// When we stretch (magnify) the image, use linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// When we minify the image, use a linear blend of two mipmaps, each filtered linearly too
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D); // Generate the mipmaps (a bunch of copies of the texture of different sizes) for optimization

	return textureID; // Return the id
}

// Static
// Loads .DDS textures. Compress using DXT1, DXT3 or DXT5.
GLuint Texture::loadDDSTexture(const std::string& texturePath)
{
	const int headerSize = 124;
	std::vector<char> header(headerSize);

	// Try to open the file
	std::ifstream file(texturePath, std::ios::binary);

	if(!file)
	{
		std::string error = "Texture '" + texturePath + "' cannot be opened!";
		Utils::CRASH(error);
		return 0;
	}

	// Verify the type of file
	std::vector<char> filecode(4);
	file.read(filecode.data(), 4);

	if(strncmp(filecode.data(), "DDS ", 4) != 0)
	{
		file.close();

		std::string error = "DDS file '" + texturePath + "' is not a correct DDS file!";
		Utils::CRASH(error);
		return 0;
	}

	// Get the surface description
	file.read(header.data(), headerSize);

	unsigned int height        = *(unsigned int*)&(header[8]);
	unsigned int width         = *(unsigned int*)&(header[12]);
	unsigned int linearSize    = *(unsigned int*)&(header[16]);
	unsigned int mipmapCount   = *(unsigned int*)&(header[24]);
	unsigned int fourCC        = *(unsigned int*)&(header[80]);

	std::vector<char> buffer;
	unsigned int bufferSize;

	// How big is it going to be, including all mipmaps?
	bufferSize = mipmapCount > 1 ? linearSize * 2 : linearSize;
	buffer.resize(bufferSize);

	file.read(buffer.data(), bufferSize);

	// Close the file
	file.close();

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
		std::string error = "DDS file '" + texturePath + "' cannot be loaded as DDS file!";
		Utils::CRASH(error);
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
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer.data() + offset);

		offset += size;
		width /= 2;
		height /= 2;

		// Deal with non-power-of-two textures
		if(width < 1) width = 1;
		if(height < 1) height = 1;
	}

	return textureID;
}

std::string Texture::getName() const
{
	return mName;
}

GLuint Texture::getID() const // Can be called from const instances
{
	return mID;
}

GLuint Texture::getType() const
{
	return mType;
}