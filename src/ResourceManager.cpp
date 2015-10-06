// Copyright 2015 Carl Hewett

// This file is part of SDL3D.

// SDL3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// SDL3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.

#include <ResourceManager.hpp>
#include <fstream>
#include <HelperFunctions.hpp>

// Shaders must be ASCII
// Reference: http://duriansoftware.com/joe/An-intro-to-modern-OpenGL.-Chapter-2.2:-Shaders.html

// OpenGL IDs are constant

using namespace HelperFunctions;

ResourceManager::ResourceManager(const std::string& resourceDir)
{
	mResourceDir = resourceDir;
}

ResourceManager::~ResourceManager()
{
	clearShaders();
}

// Returns the full resource oath
std::string ResourceManager::getFullResourcePath(const std::string& resourcePath)
{
	return mResourceDir + resourcePath;
}

// Factory
ResourceManager::shaderPointer ResourceManager::addShader(const std::string& shaderName, const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
	std::string vertexShaderPath = getFullResourcePath(vertexShaderFile); // All resources are in the resource dir
	std::string fragmentShaderPath = getFullResourcePath(fragmentShaderFile);

	shaderPointer shader(new Shader(shaderName, vertexShaderPath, fragmentShaderPath)); // Create a smart pointer of a shader instance

	shaderMapPair shaderPair(shaderName, shader);
	std::pair<shaderMap::iterator, bool> newlyAddedPair = mShaders.insert(shaderPair);
	
	if(newlyAddedPair.second == false) // It already exists in the map
	{
		std::string error = shaderName;
		error = "Shader '" + error + "' already exists and cannot be added again!";
		crash(error);
		return newlyAddedPair.first->second; // Returns a reference to the shader that was there before
	}

	return newlyAddedPair.first->second; // Returns a reference. Get the pair at pair.first, then the value at ->second
}

ResourceManager::shaderPointer ResourceManager::findShader(const std::string& shaderName) // Returns an lvalue reference, so you can modify it in the map. Make sure you keep it in the map though!
{
	// http://www.cplusplus.com/reference/unordered_map/unordered_map/find/
	shaderMap::iterator got = mShaders.find(shaderName); // Non-const iterator, since we want a non-const reference!

	if(got==mShaders.end())
	{
		std::string error = shaderName;
		error = "Shader '" + error + "' not found!";
		crash(error);
		return got->second; // Can't return nothing here!
	}

	return got->second; // Dunno why you need ->
}

void ResourceManager::clearShaders() // For freeing memory, you don't have to call this when quitting
{
	mShaders.clear(); // Clears all shaders (if you want to know, calls all deconstructors)
}

ResourceManager::texturePointer ResourceManager::addTexture(const std::string& name, const std::string& textureFile, int type)
{
	std::string path = getFullResourcePath(textureFile);

	texturePointer texture(new Texture(name, path, type));
	textureMapPair texturePair(name, texture);

	std::pair<textureMap::iterator, bool> newlyAddedPair = mTextures.insert(texturePair); // Insert in map
	
	if(newlyAddedPair.second == false)
	{
		std::string error = name;
		error = "Texture '" + error + "' already exists and cannot be added again!";
		crash(error);
		return newlyAddedPair.first->second; // Returns a reference to the texture that was there before
	}

	return newlyAddedPair.first->second; // Returns a reference to the texture
}

// The texture will take the name of the texture file (characters before the first dot). This will make it easier to add many textures.
ResourceManager::texturePointer ResourceManager::addTexture(const std::string& textureFile, int type)
{
	std::string path = getFullResourcePath(textureFile);
	std::string name;
	std::size_t firstDot = textureFile.find('.'); // Returns the index of the first found dot

	if(firstDot == std::string::npos) // No dot in this file
		name = textureFile; // Set the name as the whole file name
	else
		name = textureFile.substr(0, firstDot); // The index of the first dot aka the length of the name

	return addTexture(name, textureFile, type); // Create the texture and return it
}

ResourceManager::texturePointer ResourceManager::findTexture(const std::string& textureName)
{
	textureMap::iterator got = mTextures.find(textureName); // Const iterator, we should not need to change this GLuint

	if(got == mTextures.end())
	{
		std::string error = textureName;
		error = "Texture '" + error + "' not found!";
		crash(error);
		return got->second;
	}

	return got->second;
}

void ResourceManager::clearTextures() // For free memory
{
	mTextures.clear();
}