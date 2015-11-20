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

#version 330 core

out vec3 color;

// Interpolated values from the vertex shader
in vec2 UV;
in vec3 fPosition;
in vec3 fNormal;

// Values that stay constant for the whole mesh
uniform sampler2D textureSampler;

// lightDir, lightIntensity, Ka, Kd, Ks, shininess
vec2 blinnPhongDir(vec3 lightDir, float lightIntensity, float diffuseIntensity, float diffuseBrightness, float specularIntensity, float shininess)
{
	vec3 s = normalize(lightDir);
	vec3 v = normalize(-fPosition);
	vec3 n = normalize(fNormal);
	vec3 h = normalize(v + s);
	
	float diffuse = diffuseIntensity + diffuseBrightness * lightIntensity * max(0.0, dot(n, s));
	float specular = specularIntensity * pow(max(0.0, dot(n, h)), shininess);
	
	return vec2(diffuse, specular);
}

void main()
{	
	// DEBUG for now
	vec3 lightDir = vec3(0.1, 2.5, 5.0);
	float lightIntensity = 1.0;
	float diffuseIntensity = 0.1;
	float diffuseBrightness = 1.0;
	float specularIntensity = 1.5;
	float shininess = 10.0;
	
	vec2 light = blinnPhongDir(lightDir, lightIntensity, diffuseIntensity, diffuseBrightness, specularIntensity, shininess);
	vec3 textureColor = texture(textureSampler, UV).rgb;
	
	color = textureColor * vec3(light, 1.0);
}