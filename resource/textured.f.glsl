#version 330 core

// Interpolated values from the vertex shader
in vec2 UV;
out vec3 color;

// Values that stay constant for the whole mesh
uniform sampler2D textureSampler;

void main()
{
	// Ouput color = color at that specific UV
	color = texture(textureSampler, vec2(UV.x, 1.0-UV.y)).rgb;
}