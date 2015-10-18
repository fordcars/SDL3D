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
class GLBuffer // A simple general auto-binding OpenGL buffer wrapper
{
private:
	GLuint mID; // OpenGL handle
	bool mAutoBind;
	GLenum mTarget; // The target to bind to

public:
	// Even if auto binding is not on, calling bind() will still bind to the default target
	GLBuffer(bool autoBind = true, GLenum target = GL_ARRAY_BUFFER)
	{
		mAutoBind = autoBind;

		setTarget(target);
		glGenBuffers(1, &mID); // 1 for 1 buffer
	}

	~GLBuffer()
	{
		glDeleteBuffers(1, &mID);
	}

	// Copy constructor, makes a new OpenGL buffer. Unbinds copy buffers!
	GLBuffer(const GLBuffer& other)
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

	const GLuint getID()
	{
		return mID;
	}

	void setTarget(GLenum target)
	{
		mTarget = target;
	}

	void bind(GLenum target) const
	{
		if(mAutoBind)
			glBindBuffer(target, mID);
	}

	void bind() const // Bind to the stored target
	{
		bind(mTarget);
	}

	size_t getSize() const // Returns the buffer's size, in bytes. size_t is not in the std namespace since it is C, not C++.
	{
		GLint GLintBufferSize;

		bind();
		glGetBufferParameteriv(mTarget, GL_BUFFER_SIZE, &GLintBufferSize);

		return GLintBufferSize; // Implicit conversion to size_t
	}

	int getLength() const // Get the amount of elements in the buffer
	{
		int bufferSize = getSize();

		return bufferSize / sizeof(bufferDataType);
	}

	void setMutableData(const std::vector<bufferDataType>& data, GLenum usage)
	{
		bind();
		glBufferData(mTarget, sizeof(bufferDataType) * data.size(), &data[0], usage);
	}

	void setImmutableData(const std::vector<bufferDataType>& data, GLenum immutableFlags) // immutableFlags being a bitwise operation
	{
		bind();
		glBufferStorage(mTarget, sizeof(bufferDataType) * data.size(), &data[0], immutableFlags);
	}

	std::vector<bufferDataType> readData(GLintptr offset, GLsizeiptr size) const
	{
		std::vector<bufferDataType> data;

		bind();
		glGetBufferSubData(mTarget, offset, getSize(), &data[0]);

		return data;
	}

	// Read all of the data
	std::vector<bufferDataType> readData() const
	{
		return readData(mTarget, 0, getSize());
	}

	// If the buffer is immutable, make sure you gave the right immutableFlags to make it changeable
	void modifyData(GLintptr offset, const std::vector<bufferDataType>& data)
	{
		bind();
		glBufferSubData(mTarget, offset, HelperFunctions::getSizeOfVectorData(vertices), &data[0]);
	}
};

#endif /* GLBUFFER_H_ */