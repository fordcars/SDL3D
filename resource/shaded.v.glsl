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

#version 330 core

// Input vertex data, different for all executions
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 1) in vec3 vertexNormal_modelspace;

// Values that stay constant for the whole mesh
uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

// Output data
out vec2 UV; // Proxy, sends UV coord to fragment shader
out vec3 normal_cameraspace;
out vec3 lightDirection_cameraspace;
out vec3 vertexPosition_worldspace;

void main()
{
	//DEBUG
	vec3 lightPosition_worldspace = vec3(4, 4, 4);
	
	// UV of the vertex
	UV = vertexUV;
	
	vertexPosition_worldspace = (modelMatrix * vec4(vertexPosition_modelspace, 1)).xyz;
	
	vec3 vertexPosition_cameraspace = (viewMatrix * modelMatrix * vec4(vertexPosition_modelspace, 1)).xyz;
	vec3 eyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;
	
	vec3 lightPosition_cameraspace = (viewMatrix * vec4(lightPosition_worldspace, 1)).xyz;
	lightDirection_cameraspace = lightPosition_cameraspace + eyeDirection_cameraspace; // Vector from vertex to light
	
	normal_cameraspace = (normalMatrix * modelMatrix * vec4(vertexNormal_modelspace, 0.0)).xyz;
	
	// Output position of the vertex
	gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
}