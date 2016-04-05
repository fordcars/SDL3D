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

// A simple general auto-binding OpenGL buffer wrapper
// I am pretty sure constantly rebinding a buffer isn't too bad
// Useful for using as an interface for other classes (retun this instead of writing an interface)

// Every function that calls OpenGL stuff must call bind() first

#ifndef GPU_BUFFER_HPP
#define GPU_BUFFER_HPP

#include <glad/glad.h> // glad.h is compatible with C++
#include <vector>
#include <cstddef> // For std::size_t

template<typename bufferDataType>
class GPUBuffer
{
private:
	GLuint mID; // OpenGL handle
	bool mAutoBind;
	GLenum mTarget; // The target to bind to

public:
	// Even if auto binding is not on, calling bind() will still bind to the default target
	GPUBuffer(GLenum target = GL_ARRAY_BUFFER, bool autoBind = true)
	{
		setTarget(target);
		mAutoBind = autoBind;

		glGenBuffers(1, &mID); // 1 for 1 buffer
	}

	~GPUBuffer()
	{
		glDeleteBuffers(1, &mID);
	}

	// Copy constructor, makes a new OpenGL buffer. Unbinds copy buffers!
	GPUBuffer(const GPUBuffer& other)
	{
		// For newer OpenGL versions: int oldCopyWriteBuffer = glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING);
		// int oldCopyReadBuffer = glGetIntegerv(GL_COPY_READ_BUFFER_BINDING);

		mAutoBind = other.mAutoBind;
		setTarget(other.mTarget);
		
		glGenBuffers(1, &mID);

		// Code to make sure the data and the flags are the same
		GLint isImmutable;
		other.bind();
		glGetBufferParameteriv(other.mTarget, GL_BUFFER_IMMUTABLE_STORAGE, &isImmutable);
		
		if(isImmutable)
		{
			GLint immutableFlags;
			other.bind();
			glGetBufferParameteriv(other.mTarget, GL_BUFFER_STORAGE_FLAGS, &immutableFlags);
			
			setImmutableData(other.read(), immutableFlags);
		} else
		{
			GLint usage;
			other.bind();
			glGetBufferParameteriv(other.mTarget, GL_BUFFER_USAGE, &usage);

			setMutableData(other.read(), usage);
		}
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

	std::size_t getSize() const // Returns the buffer's size, in bytes
	{
		GLint GLintBufferSize;

		bind();
		glGetBufferParameteriv(mTarget, GL_BUFFER_SIZE, &GLintBufferSize);

		return GLintBufferSize; // Implicit conversion to std::size_t
	}

	int getLength() const // Get the amount of elements in the buffer
	{
		int bufferSize = getSize();

		return bufferSize / sizeof(bufferDataType);
	}

	void setMutableData(const std::vector<bufferDataType>& data, GLenum usage)
	{
		bind();

		// Vector.size() returns the amount of elements
		glBufferData(mTarget, sizeof(bufferDataType) * data.size(), data.data(), usage);
	}

	// Uses defaut usage, easier Lua binding have a function overload to do it
	// instead of a default C++ argument (=value in parameter definition)
	void setMutableData(const std::vector<bufferDataType>& data)
	{
		setMutableData(data, GL_DYNAMIC_DRAW); // Binding is done in there
	}

	void setImmutableData(const std::vector<bufferDataType>& data, GLenum immutableFlags) // immutableFlags being a bitwise operation
	{
		bind();
		glBufferStorage(mTarget, sizeof(bufferDataType) * data.size(), data.data(), immutableFlags);
	}

	// Easier Lua binding, see setMutableData(data)
	void setImmutableData(const std::vector<bufferDataType>& data)
	{
		// Binding is done in there:
		setImmutableData(data, GL_DYNAMIC_STORAGE_BIT); // Best flag I could find?
	}

	std::vector<bufferDataType> read(GLintptr offset, GLsizeiptr size) const
	{
		bind();
		std::vector<bufferDataType> data(size / sizeof(bufferDataType)); // Allocate
		glGetBufferSubData(mTarget, offset, size, data.data());

		return data;
	}

	// Read all of the data
	std::vector<bufferDataType> read() const
	{
		return read(0, getSize());
	}

	// If the buffer is immutable, make sure you gave the right immutableFlags to make it changeable
	// Will replace the bytes starting at offset
	void modify(GLintptr offset, const std::vector<bufferDataType>& data)
	{
		bind();
		glBufferSubData(mTarget, offset, sizeof(bufferDataType) * data.size(), data.data());
	}

};

#endif /* GPU_BUFFER_HPP */