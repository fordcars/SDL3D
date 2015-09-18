#ifndef OBJECT_H_
#define OBJECT_H_

#include <GLAD/glad.h>
#include <glm/glm.hpp>
#include <memory>

#include <Shader.h>
#include <Definitions.h>

// This is the base class of 3d objects. This class holds the vertices.
// This class can also live on it's own.
// Eventually, we could have 1 vertex buffer holding all of the objects.

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

class Object
{
protected: // Only accessible to this class and derived classes
	typedef std::shared_ptr<const Shader> shaderPointer; // Useful for derived classes, too

	GLuint getVertexBuffer() const { return mVertexBuffer; } // Useful for render functions, these are read only!
	int getNumberOfVertices() const { return mNumberOfVertices; } // Useful for render functions, these are read only!

private:
	GLuint mVertexBuffer; // Holds vertices
	int mNumberOfVertices;

	shaderPointer mShader; // The shader used to render this object

public:
	Object(GLfloatArray vertices, int numberOfVertices, shaderPointer shader);
	~Object();

	void setShader(shaderPointer shader);
	shaderPointer getShader();

	virtual void render(glm::mat4 MVP); // Overload this if you need to!
};

#endif /* OBJECT_H_ */