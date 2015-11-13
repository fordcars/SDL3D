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

#ifndef BASICOBJECT_HPP_
#define BASICOBJECT_HPP_

#include <ObjectTemplate.hpp>
#include <glm/glm.hpp>
#include <GLAD/glad.h> // OpenGL, rendering and all

class BasicObject : public ObjectTemplate
{
protected:
	constShaderPointer getShader();

private:
	constShaderPointer mShaderPointer; // The shader used to render this object, pointer.

public:
	BasicObject(const ObjectTemplate& objectTemplate, constShaderPointer shaderPointer);
	~BasicObject();

	void setShader(constShaderPointer shaderPointer);

	virtual void render(glm::mat4 MVP); // Overload this if you need to!
};

#endif /* BASICOBJECT_HPP_ */