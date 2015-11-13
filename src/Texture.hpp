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

#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

// For DDS files
#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844

#include <string>
#include <GLAD/glad.h>

// Since I am not feeling like rewriting OpenGL, this class is more of a datatype with functions

class Texture
{
private:
	std::string mName; // May be useful, for error messages for example
	int mType;

	GLuint mID;

	static const GLuint loadBMPTexture(const std::string& texturePath);
	static const GLuint loadDDSTexture(const std::string& texturePath);

public:
	Texture(const std::string& name, const std::string& texturePath, int type);
	~Texture();

	const GLuint getID() const;
	const GLuint getType() const;
};

#endif /* TEXTURE_HPP_ */