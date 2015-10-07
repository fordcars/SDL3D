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

// This is the base class of 3d objects. This class holds the vertices.
// This class can also live on it's own.
// Eventually, we could have 1 vertex buffer holding all of the objects.

#ifndef OBJECT_H_
#define OBJECT_H_

#include <GLAD/glad.h>
#include <glm/glm.hpp>
#include <memory>

#include <Shader.hpp>
#include <Definitions.hpp>

#include <GLBuffer.hpp>

class Object
{
protected: // Only accessible to this class and derived classes
	typedef GLBuffer<GLfloat> GLfloatBuffer;
	typedef std::vector<GLfloat> GLfloatVector;
	typedef std::shared_ptr<const Shader> shaderPointer; // Useful for derived classes, too

	static void loadOBJData(const std::string& filePath);

private:
	GLfloatBuffer mVertexBuffer; // The OpenGL vertex buffer
	shaderPointer mShaderPointer; // The shader used to render this object, pointer.

public:
	Object(GLfloatVector &vertices, shaderPointer shaderPointer);
	~Object();

	GLfloatBuffer &getVertexBuffer();

	void setShader(shaderPointer shaderPointer);
	shaderPointer getShader();

	virtual void render(glm::mat4 MVP); // Overload this if you need to!
};

#endif /* OBJECT_H_ */