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

#ifndef TEXTUREDOBJECT_H_
#define TEXTUREDOBJECT_H_

#include <Object.hpp>
#include <Texture.hpp>
#include <memory> // For smart pointers

class TexturedObject : public Object // Inherit! 'public' is required here
{
private:
	typedef std::shared_ptr<const Texture> constTexturePointer; // We can't modify the texture

	constTexturePointer mTexture; // Non-const so we can change the texture
	GLuint mUVBuffer;

public:
	TexturedObject(GLfloatVector vertices, GLfloatVector UVCoords, shaderPointer shader, constTexturePointer texture);
	~TexturedObject();

	void setTexture(constTexturePointer texture);
	void render(glm::mat4 MVP); // Overloading
};

#endif /* TEXTURED_OBJECT_H_ */