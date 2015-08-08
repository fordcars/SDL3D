#version 330 core

// Interpolated values from the vertex shader
in vec2 UV;

// Values that stay constant for the whole mesh
uniform sampler2D textureSampler;

void main()
{
	// Ouput color = color at that specific UV
	gl_FragColor = texture(textureSampler, UV);
}