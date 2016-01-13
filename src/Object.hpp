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

#ifndef OBJECT_HPP_
#define OBJECT_HPP_

#include <Entity.hpp>
#include <ObjectGeometry.hpp>
#include <Camera.hpp>

#include <glm/glm.hpp>
#include <glad/glad.h> // OpenGL, rendering and all

class Object : public Entity
{
public:
	typedef std::shared_ptr<const Shader> constShaderPointer; // Const shader

private:
	ObjectGeometry mObjectGeometry;
	constShaderPointer mShaderPointer; // The shader used to render this object, pointer.

public:
	Object(const ObjectGeometry& objectGeometry, constShaderPointer shaderPointer);
	virtual ~Object() override; // virtual here is just for looks

	ObjectGeometry& getObjectGeometry();

	void setShader(constShaderPointer shaderPointer);
	constShaderPointer getShader();

	virtual void render(const Camera& camera); // Override this if you need to!
};

#endif /* OBJECT_HPP_ */