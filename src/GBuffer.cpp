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

#include "GBuffer.hpp"

#include "Utils.hpp"
#include <cstddef> // For std::size_t

// All color textures will be drawable
GBuffer::GBuffer(glm::ivec2 size, int colorTextureCount)
{
	mSize = size;
	mDepthTextureID = 0;

	init(colorTextureCount);
}

GBuffer::~GBuffer()
{
}

bool GBuffer::init(int colorTextureCount)
{
	if(colorTextureCount > GL_MAX_DRAW_BUFFERS)
	{
		Utils::CRASH("Error! Color texture count requested for G-buffer exceeds the maximum amount allowed "
			"by your system!");
		return false;
	}

	std::vector<GLenum> drawBuffers;

	// Bind the framebuffer
	bind(GL_DRAW_FRAMEBUFFER);

	// Create the color textures
	glGenTextures(colorTextureCount, mColorTextureIDs.data());

	for(std::size_t i = 0; i < mColorTextureIDs.size(); i++)
	{
		GLuint ID = mColorTextureIDs[i];
		GLenum colorAttachment = GL_COLOR_ATTACHMENT0 + i;

		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB32F,
			mSize.x,
			mSize.y,
			0,
			GL_RGB,
			GL_FLOAT,
			nullptr);
		
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, colorAttachment, GL_TEXTURE_2D, ID, 0);

		drawBuffers.push_back(colorAttachment);
	}

	// Create the depth texture
	glGenTextures(1, &mDepthTextureID);
	glBindTexture(GL_TEXTURE_2D, mDepthTextureID);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_DEPTH_COMPONENT32F,
		mSize.x,
		mSize.y,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT,
		nullptr);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTextureID, 0);

	// Setup draw buffers
	glDrawBuffers(drawBuffers.size(), drawBuffers.data());

	// Check for errors!
	checkFramebufferCompleteness(GL_DRAW_FRAMEBUFFER);

	// Rebind the default framebuffer. Future framebuffer operations will affect this frame buffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}