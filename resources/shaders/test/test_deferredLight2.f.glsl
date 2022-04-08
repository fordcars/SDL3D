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
		
	color = fragNormal_cameraspace;
}