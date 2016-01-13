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

// Per vertex lighting

#version 330 core

// Interpolated values from the vertex shader
in vec2 UV;
in vec3 normal_cameraspace;
in vec3 lightDirection_cameraspace;
in vec3 vertexPosition_worldspace;
in vec3 eyeDirection_cameraspace;

out vec3 color;

// Values that stay constant for the whole mesh
uniform sampler2D textureSampler;
//uniform vec3 lightColor;
//uniform float lightPower

void main()
{
	//DEBUG
	vec3 lightPosition_worldspace = vec3(400, 400, 400);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float lightPower = 300000.0;
	
	vec3 textureColor = texture(textureSampler, UV).rgb;

	vec3 materialDiffuseColor = textureColor;
	vec3 materialAmbientColor = vec3(0.5, 0.5, 0.5) * materialDiffuseColor;
	vec3 materialSpecularColor = vec3(1.0, 1.0, 1.0);
	
	float squareDistance = pow(length(lightPosition_worldspace - vertexPosition_worldspace), 2);
	
	vec3 n = normalize(normal_cameraspace); // Normal of fragment
	vec3 ld = normalize(lightDirection_cameraspace); // Direction of the light (from the fragment to the light)
	
	float cosTheta = clamp(dot(n, ld), 0, 1); // Always positive! Otherwise we have a negative color.
	
	// From vertex towards the camera
	vec3 E = normalize(eyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-ld, n);
	float cosAlpha = clamp(dot(E, R), 0, 1);
	
	color = 
	// Ambient : simulates indirect lighting
	materialAmbientColor +
	// Diffuse : "color" of the object
	// In GLSL, multiplications are just the multiplications of the vector's components
	materialDiffuseColor * lightColor * lightPower * cosTheta / squareDistance +
	// Specular " reflective highlight, like a mirror
	// Multiplying by cos theta removes annoying artefacts http://www.gamedev.net/topic/672374-blinn-phong-artifact-in-shader/
	materialSpecularColor * lightColor * lightPower * pow(cosAlpha, 5) / squareDistance * cosTheta;
}