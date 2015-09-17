#version 330 core

// Interpolated values from the vertex shader
in vec2 UV;
out vec3 color;

// Values that stay constant for the whole mesh
uniform sampler2D textureSampler;
uniform int textureType;

void main()
{
	// Ouput color = color at that specific UV
	switch(textureType)
	{
		case 1: // BMP
			color = texture(textureSampler, UV).rgb;
			break;
			
		case 2: // DDS
			color = texture(textureSampler, vec2(UV.x, 1.0-UV.y)).rgb;
			break;
			
		default:
			color = texture(textureSampler, UV).rgb;
			break;
	}
}