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

#ifndef SHADED_OBJECT_HPP
#define SHADED_OBJECT_HPP

#include <TexturedObject.hpp>
#include <ObjectGeometry.hpp>
#include <Texture.hpp>
#include <Camera.hpp>

#include <memory> // For smart pointers

class ShadedObject : public TexturedObject // Inherit! 'public' makes the TexturedObject interface public.
{
public:
	ShadedObject(constObjectGeometryPointer objectGeometry, constShaderPointer shaderPointer, constTexturePointer texturePointer,
		bool physicsCircularShape, int physicsType);
	~ShadedObject() override;

	void render(const Camera& camera) override;
};

#endif /* SHADED_OBJECT_HPP */