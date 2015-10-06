// Copyright 2015 Carl Hewett

// This file is part of SDL3D.

// SDL3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// SDL3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#define LOG_FILE "Log.txt"

// Texture types
#define BMP_TEXTURE 1
#define DDS_TEXTURE 2

// For DDS files
#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844

#include <GLAD/glad.h>
#include <array>
#include <unordered_map>

// Useful typedefs
typedef std::unordered_map<std::string, GLuint> GLuintMap;
typedef std::pair<std::string, GLuint> GLuintMapPair;

#endif /* DEFINITIONS_H_ */