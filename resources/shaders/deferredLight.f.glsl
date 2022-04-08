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

// From: https://learnopengl.com/Advanced-Lighting/Deferred-Shading

#version 330 core

in vec3 lightPos_cameraspace;
in vec2 UV;

out vec3 color;

uniform mat4 viewMatrix;

uniform vec3 lightPos_worldspace;
uniform vec3 lightDiffuseColor;
uniform vec3 lightSpecularColor;
uniform float lightIntensity;

uniform sampler2D positionTex;
uniform sampler2D normalTex;
uniform sampler2D albedoTex;

void main()
{
    vec3 fragPos_worldspace = texture(positionTex, UV).rgb;
	vec3 fragNormal_cameraspace = texture(normalTex, UV).rgb;
	vec3 albedo = texture(albedoTex, UV).rgb;
    
	vec3 fragPos_cameraspace = (viewMatrix * vec4(fragPos_worldspace, 1)).xyz;
	vec3 eyeDirection_cameraspace = vec3(0, 0, 0) - fragPos_cameraspace;               // Frag to camera
	vec3 lightDirection_cameraspace = lightPos_cameraspace + eyeDirection_cameraspace; // Frag to light
    
	float squareDistance = pow(length(lightPos_worldspace - fragPos_worldspace), 2);
	
	vec3 n = fragNormal_cameraspace;
	vec3 ld = normalize(lightDirection_cameraspace);
	
	float cosTheta = clamp(dot(n, ld), 0, 1);        // Always positive! Otherwise we have a negative color.
	
	vec3 E = normalize(eyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-ld, n);
	float cosAlpha = clamp(dot(E, R), 0, 1);
	
	color =
		// Diffuse
		albedo * lightDiffuseColor * lightIntensity * cosTheta / squareDistance +
		// Specular
		// Multiplying by cos theta removes annoying artefacts http://www.gamedev.net/topic/672374-blinn-phong-artifact-in-shader/
		lightSpecularColor * lightDiffuseColor * lightIntensity * pow(cosAlpha, 5) / squareDistance * cosTheta;
}