#version 330 core

varying vec3 fScreenPos;

void main()
{
	gl_FragColor = vec4(fScreenPos, 1);
}