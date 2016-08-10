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
#define MAX_LIGHTS 128

// Interpolated values from the vertex shader
in vec2 UV;
in vec3 normal_cameraspace;
in vec3 vertexPosition_worldspace; // Same as gl_FragCoord, but in world space instead of clip space
in vec3 eyeDirection_cameraspace; // Vector from fragment to camera
//////////////////////////////////////////////// TODOOO Filter out lights that are too far
out vec3 color;

// Values that stay constant for the whole mesh
uniform mat4 viewMatrix;
uniform sampler2D textureSampler;
uniform int lightCount;

struct Light
{
	vec3 position;
	vec3 diffuseColor;
	vec3 specularColor;
	float power;
	float isOn;
};

layout(std140) uniform Lights
{
	Light lights[MAX_LIGHTS];
};

void main()
{
	vec3 textureColor = texture(textureSampler, UV).rgb;

	vec3 materialDiffuseColor = textureColor;
	vec3 materialAmbientColor = vec3(0.4) * materialDiffuseColor;
	vec3 materialSpecularColor = vec3(1.0);

	vec3 lightAmbientColor = vec3(0.0); // All light diffuse colors are combined in this for more realistic ambient lighting
	
	for(int i = 0; i < lightCount; i++)
	{
		vec3 lightPosition_worldspace = lights[i].position;
		vec3 diffuseColor = lights[i].diffuseColor;
		vec3 specularColor = lights[i].specularColor; // Normally this is white
		float lightPower = lights[i].power;
		
		// Light calculations are normally done in camera space for easier specular math
		vec3 lightPosition_cameraspace = (viewMatrix * vec4(lightPosition_worldspace, 1)).xyz;
		vec3 lightDirection_cameraspace = normalize(lightPosition_cameraspace + eyeDirection_cameraspace); // Vector from fragment to light
		
		if(lights[i].isOn == 0.0 || lights[i].power == 0.0)
			continue; // If it is off, ignore this light
		
		////
		
		float squareDistance = pow(length(lightPosition_worldspace - vertexPosition_worldspace), 2);
		
		vec3 n = normalize(normal_cameraspace); // Normal of fragment
		float cosTheta = clamp(dot(n, lightDirection_cameraspace), 0, 1); // Always positive! Otherwise we have a negative fragment color.
		
		// From vertex towards the camera
		vec3 E = normalize(eyeDirection_cameraspace);
		// Direction in which the triangle reflects the light
		vec3 R = reflect(-lightDirection_cameraspace, n);
		float cosAlpha = clamp(dot(E, R), 0, 1);
		
		////
		
		lightAmbientColor += diffuseColor;
		
		// Add our new color onto the color of previous light passes
		color +=
		// Diffuse : "color" of the object
		// In GLSL, multiplications are just the multiplications of the vector's components
		materialDiffuseColor * diffuseColor * lightPower * cosTheta / squareDistance +
		// Specular " reflective highlight, like a mirror
		// Multiplying by cos theta removes annoying artefacts http://www.gamedev.net/topic/672374-blinn-phong-artifact-in-shader/
		materialSpecularColor * specularColor * lightPower * pow(cosAlpha, 5) / squareDistance * cosTheta;
	}
	
	color +=
		// Ambient : simulates indirect lighting
		materialAmbientColor * clamp(lightAmbientColor, 0.0, 1.0); // Clamp to make sure we do not make the ambient lighting brighter
}