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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Entity.hpp"
#include "ObjectGeometry.hpp"
#include "Camera.hpp"

#include "glm/glm.hpp"
#include "glad/glad.h" // OpenGL, rendering and all

class Object : public Entity
{
public:
	using constObjectGeometryPointer = std::shared_ptr<const ObjectGeometry>;
	using constShaderPointer = std::shared_ptr<const Shader>; // Const shader

private:
	constObjectGeometryPointer mObjectGeometry;
	constShaderPointer mShaderPointer; // The shader used to render this object, pointer.

public:
	Object(constObjectGeometryPointer objectGeometry, constShaderPointer shaderPointer,
		bool physicsCircularShape, int physicsType);
	virtual ~Object() override; // virtual here is just for looks

	void setObjectGeometry(constObjectGeometryPointer objectGeometry);
	constObjectGeometryPointer getObjectGeometry() const;

	void setShader(constShaderPointer shaderPointer);
	constShaderPointer getShader() const;

	virtual void render(const Camera& camera); // Override this if you need to!
};

#endif // OBJECT_HPP