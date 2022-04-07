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

// This file is heavily based off http://www.opengl-tutorial.org/, see SpecialThanks.txt

#version 330 core

layout(location = 0) in vec3 vertexPosition_clipspace;
layout(location = 1) in vec2 vertexUV;

uniform mat4 viewMatrix;

out vec2 UV;

void main()
{
	// Output position of the vertex
	gl_Position = vec4(vertexPosition_clipspace, 1);
	
	// UV of the vertex
	UV = vertexUV;
}