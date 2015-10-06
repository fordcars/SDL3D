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

#include <GLBuffer.hpp>

template<typename bufferDataType>
GLBuffer<bufferDataType>::GLBuffer()
{
	glGenBuffers(1, &mID); // 1 for 1 buffer
}

template<typename bufferDataType>
GLBuffer<bufferDataType>::~GLBuffer()
{
	glDeleteBuffers(1, &mID);
}

// Copy constructor, makes a new OpenGL buffer. Unbinds copy buffers!
template<typename bufferDataType>
GLBuffer<bufferDataType>::GLBuffer(const GLBuffer &other)
{
	// For newer OpenGL versions: int oldCopyWriteBuffer = glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING);
	// int oldCopyReadBuffer = glGetIntegerv(GL_COPY_READ_BUFFER_BINDING);

	glGenBuffers(1, &mID); // Create a buffer
	bind(GL_COPY_WRITE_BUFFER); // Bind it as the write buffer

	other.bind(GL_COPY_READ_BUFFER);

	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, other.getSize()); // Let OpenGL copy it

	// Unbind them for cleanliness
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
}

template<typename bufferDataType>
const GLuint GLBuffer<bufferDataType>::getID() const
{
	return mID;
}

template<typename bufferDataType>
void GLBuffer<bufferDataType>::bind(GLenum target) const
{
	glBindBuffer(target, mID);
}

template<typename bufferDataType>
std::size_t GLBuffer<bufferDataType>::getSize(GLenum target) const // Returns the buffer's size, in bytes
{
	std::size_t bufferSize;
	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);

	return bufferSize;
}

template<typename bufferDataType>
int GLBuffer<bufferDataType>::getLength(GLenum target) const // Get the amount of elements in the buffer
{
	std::size_t bufferSize = getSize(target);

	return bufferSize / sizeof(bufferDataType);
}

template<typename bufferDataType>
void GLBuffer<bufferDataType>::setMutableData(GLenum target, const std::vector<bufferDataType> &data, GLenum usage)
{
	glBufferData(target, HelperFunctions::sizeOfVectorData(vertices), &mVertices[0], usage);
}

template<typename bufferDataType>
void GLBuffer<bufferDataType>::setImmutableData(GLenum target, const std::vector<bufferDataType> &data, GLenum immutableFlags) // immutableFlags being a bitwise operation
{
	glBufferStorage(target, HelperFunctions::sizeOfVectorData(data), &data[0], immutableFlags);
}

template<typename bufferDataType>
std::vector<bufferDataType> GLBuffer<bufferDataType>::readData(GLenum target, GLintptr offset, GLsizeiptr size) const
{
	std::vector<bufferDataType> data;

	glGetBufferSubData(target, offset, HelperFunctions::sizeOfVectorData(vertices), &data[0]);

	return data;
}

// Read all of the data
template<typename bufferDataType>
std::vector<bufferDataType> GLBuffer<bufferDataType>::readData(GLenum target) const
{
	return readData(target, 0, getSize());
}

// If the buffer is immutable, make sure you gave the right immutableFlags to make it changeable
template<typename bufferDataType>
void GLBuffer<bufferDataType>::modifyData(GLenum target, GLintptr offset, const std::vector<bufferDataType> &data)
{
	glBufferSubData(target, offset, HelperFunctions::sizeOfVectorData(vertices), &data[0]);
}