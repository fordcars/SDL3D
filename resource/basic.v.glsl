#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
uniform mat4 MVP;

varying vec3 fScreenPos;

void main()
{
	vec4 v = vec4(vertexPosition_modelspace, 1);
	gl_Position = MVP * v;
	
	fScreenPos = vec3(gl_Position.xyz);
}