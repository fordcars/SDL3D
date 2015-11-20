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

// Based of default script of: http://shdr.bkcore.com/

#version 330 core

// Input vertex data, different for all executions
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 1) in vec3 vertexNormal;

// Values that stay constant for the whole mesh
uniform mat4 MVP;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

// Output data
out vec2 UV; // Proxy, sends UV coord to fragment shader
out vec3 fPosition;
out vec3 fNormal; // Proxy

void main()
{
	vec4 pos = modelViewMatrix * vec4(vertexPosition_modelspace, 1.0);
	vec4 normal = normalize(normalMatrix * vec4(vertexNormal, 0.0));

	// UV of the vertex
	UV = vertexUV;
	fPosition = pos.xyz; // Fragment position
	fNormal = normal.xyz;
	
	// Output position of the vertex
	gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
}