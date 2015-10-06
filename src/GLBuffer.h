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

#ifndef GLBUFFER_H_
#define GLBUFFER_H_

#include <GLAD/glad.h>
#include <vector>

#include <cstdlib> // For size_t

template<typename bufferDataType>
class GLBuffer // A simple general OpenGL buffer wrapper
{
private:
	GLuint mID; // OpenGL handle

public:
	GLBuffer();
	~GLBuffer();
	GLBuffer(const GLBuffer &other);

	const GLuint getID() const;

	void bind(GLenum target) const;

	std::size_t getSize(GLenum target) const;
	int getLength(GLenum target) const;

	void setMutableData(GLenum target, const std::vector<bufferDataType> &data, GLenum usage);
	void setImmutableData(GLenum target, const std::vector<bufferDataType> &data, GLenum immutableFlags);

	std::vector<bufferDataType> readData(GLenum target, GLintptr offset, GLsizeiptr size) const;
	std::vector<bufferDataType> readData(GLenum target) const;

	void modifyData(GLenum target, GLintptr offset, const std::vector<bufferDataType> &data);
};

#endif /* GLBUFFER_H_ */