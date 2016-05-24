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

#ifndef TEXTURED_OBJECT_HPP
#define TEXTURED_OBJECT_HPP

#include "Object.hpp"
#include "ObjectGeometry.hpp"
#include "Texture.hpp"
#include "Camera.hpp"

#include <memory> // For smart pointers

class TexturedObject : public Object // Inherit! 'public' is required here
{
public:
	using constTexturePointer = std::shared_ptr<const Texture>; // We can't modify the texture

private:
	constTexturePointer mTexturePointer; // Non-const so we can change which texture we are using

public:
	TexturedObject(constObjectGeometryPointer objectGeometry, constShaderPointer shaderPointer, constTexturePointer texturePointer,
		bool physicsCircularShape, int physicsType);
	~TexturedObject() override;

	void setTexture(constTexturePointer texturePointer);
	constTexturePointer getTexture();

	void render(const Camera& camera) override;
};

#endif // TEXTURED_OBJECT_HPP