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

#ifndef SHADEDOBJECT_HPP_
#define SHADEDOBJECT_HPP_

#include <BasicObject.hpp>
#include <ObjectGeometry.hpp>
#include <Texture.hpp>

#include <memory> // For smart pointers

class ShadedObject : public BasicObject // Inherit! 'public' is required here
{
private:
	typedef std::shared_ptr<const Texture> constTexturePointer; // We can't modify the texture
	constTexturePointer mTexturePointer; // Non-const so we can change which texture we are using

public:
	ShadedObject(const ObjectGeometry& objectGeometry, ObjectGeometry::constShaderPointer shaderPointer, constTexturePointer texturePointer);
	~ShadedObject();

	void setTexture(constTexturePointer texturePointer);
	void render(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix); // Overloading
};

#endif /* SHADEDOBJECT_HPP_ */