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

// Per fragment lighting
// Based off the blinn-phong snippet at http://shdr.bkcore.com/

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
//uniform vec3 lightPower

vec2 blinnPhongDir(vec3 lightDir, float lightInt, float diffuseIntensity, float specularIntensity, float shininess)
{
	vec3 s = normalize(lightDir);
	vec3 v = normalize(-vertexPosition_worldspace);
	vec3 n = normalize(normal_cameraspace);
	vec3 h = normalize(v+s);
	
	float diffuse = diffuseIntensity * lightInt * max(0.0, dot(n, s));
	float specular = specularIntensity * lightInt * pow(max(0.0, dot(n, h)), shininess);
	
	return vec2(diffuse, specular);
}

void main()
{
	//DEBUG
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float lightPower = 10.0;
	
	vec3 textureColor = texture(textureSampler, UV).rgb;
	
	vec3 materialDiffuseColor = textureColor;
	vec3 materialAmbientColor = vec3(0.1, 0.1, 0.1) * materialDiffuseColor;
	vec3 materialSpecularColor = vec3(1.0, 1.0, 1.0);
	
	float diffuseIntensity = 0.1;
	float specularIntensity = 0.1;
	float shininess = 10.0;
	
	vec2 lighting = blinnPhongDir(lightDirection_cameraspace, lightPower, diffuseIntensity, specularIntensity, shininess);
	
	color = 
	// Ambient : simulates indirect lighting
	materialAmbientColor +
	// Diffuse : "color" of the object
	materialDiffuseColor * lightColor * lighting.x +
	// Specular " reflective highlight, like a mirror
	materialSpecularColor * lightColor * lighting.y;
}