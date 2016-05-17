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

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

// For DDS files
#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844

#include <string>
#include <glad/glad.h>

// Since I am not feeling like rewriting OpenGL, this class is more of a datatype with functions

class Texture
{
private:
	std::string mName; // May be useful, for error messages for example. Don't change this stupidly.
	std::string mPath;
	int mType;

	GLuint mID;

	bool load();

	static GLuint loadBMPTexture(const std::string& texturePath);
	static GLuint loadDDSTexture(const std::string& texturePath);

public:
	Texture(const std::string& name, const std::string& path, int type);
	Texture(const Texture& other);
	~Texture();

	std::string getName() const;
	GLuint getID() const;
	GLuint getType() const;
};

#endif // TEXTURE_HPP