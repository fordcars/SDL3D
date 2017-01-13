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

// Binds automatically when using this interface

#include "GPUFramebuffer.hpp"
#include "Utils.hpp"

GPUFramebuffer::GPUFramebuffer()
{
	mID = 0;

	glGenFramebuffers(1, &mID);
}

GPUFramebuffer::~GPUFramebuffer()
{
	glDeleteFramebuffers(1, &mID);
}

GLuint GPUFramebuffer::getID()
{
	return mID;
}

void GPUFramebuffer::bind(GLenum target) const
{
	glBindFramebuffer(target, mID);
}

void GPUFramebuffer::attach1DTexture(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	bind(GL_DRAW_FRAMEBUFFER);
	glFramebufferTexture1D(target, attachment, textarget, texture, level);
	checkFramebufferCompleteness(GL_DRAW_FRAMEBUFFER);
}

void GPUFramebuffer::attach2DTexture(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	bind(GL_DRAW_FRAMEBUFFER);
	glFramebufferTexture2D(target, attachment, textarget, texture, level);
	checkFramebufferCompleteness(GL_DRAW_FRAMEBUFFER);
}

void GPUFramebuffer::attachTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
	bind(GL_DRAW_FRAMEBUFFER);
	glFramebufferTextureLayer(target, attachment, texture, level, layer);
	checkFramebufferCompleteness(GL_DRAW_FRAMEBUFFER);
}

// Returns true on success/completeness
bool GPUFramebuffer::checkFramebufferCompleteness(GLenum target)
{
	GLenum error = glCheckFramebufferStatus(target);

	if(error == GL_FRAMEBUFFER_UNDEFINED)
	{
		Utils::CRASH("Error, GPUFramebuffer incomplete! OpenGL error code: GL_FRAMEBUFFER_UNDEFINED");
		return false;
	}
	else if(error == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
	{
		Utils::CRASH("Error, GPUFramebuffer incomplete! OpenGL error code: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
		return false;
	}
	else if(error == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
	{
		Utils::CRASH("Error, GPUFramebuffer incomplete! OpenGL error code: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
		return false;
	}
	else if(error == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
	{
		Utils::CRASH("Error, GPUFramebuffer incomplete! OpenGL error code: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
		return false;
	}
	else if(error == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
	{
		Utils::CRASH("Error, GPUFramebuffer incomplete! OpenGL error code: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
		return false;
	}
	else if(error == GL_FRAMEBUFFER_UNSUPPORTED)
	{
		Utils::CRASH("Error, GPUFramebuffer incomplete! OpenGL error code: GL_FRAMEBUFFER_UNSUPPORTED");
		return false;
	}
	else if(error == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
	{
		Utils::CRASH("Error, GPUFramebuffer incomplete! OpenGL error code: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
		return false;
	}
	else if(error == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
	{
		Utils::CRASH("Error, GPUFramebuffer incomplete! OpenGL error code: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
		return false;
	}
	else
	{
		return true;
	}
}