//// Copyright 2017 Carl Hewett
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

#ifndef GPUFRAMEBUFFER_HPP
#define GPUFRAMEBUFFER_HPP

#include "glad/glad.h"

class GPUFramebuffer
{
private:
	GLuint mID;

public:
	GPUFramebuffer();
	virtual ~GPUFramebuffer();

	GLuint getID();
	void bind(GLenum target) const;
	void attach1DTexture(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	void attach2DTexture(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	void attachTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);

	bool checkFramebufferCompleteness(GLenum target);
};

#endif // GPUFRAMEBUFFER_HPP