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

#include <ResourceManager.hpp>
#include <fstream>
#include <Utils.hpp>

// Shaders must be ASCII or UTF-8
// Reference: http://duriansoftware.com/joe/An-intro-to-modern-OpenGL.-Chapter-2.2:-Shaders.html

// OpenGL IDs are constant

// Texture and shader names are also kept in their instances

ResourceManager::ResourceManager(const std::string& resourceDir)
{
	mResourceDir = resourceDir;
}

ResourceManager::~ResourceManager()
{
	clearShaders();
}

// Static
// Get the name of the file before the first dot '.'
std::string ResourceManager::getBasename(const std::string& file)
{
	std::size_t firstDot = file.find('.'); // Returns the index of the first found dot

	if(firstDot == std::string::npos) // No dot in this file
		return file; // Set the name as the whole file name
	else
		return file.substr(0, firstDot); // The index of the first dot aka the length of the name
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

	shaderPointer shaderPointer(new Shader(shaderName, vertexShaderPath, fragmentShaderPath)); // Create a smart pointer of a shader instance

	shaderMapPair shaderPair(shaderName, shaderPointer);
	std::pair<shaderMap::iterator, bool> newlyAddedPair = mShaderMap.insert(shaderPair);
	
	if(newlyAddedPair.second == false) // It already exists in the map
	{
		std::string error = "Shader '" + shaderName + "' already exists and cannot be added again!";;
		Utils::crash(error, __LINE__, __FILE__);
		return newlyAddedPair.first->second; // Returns a reference to the shader that was there before
	}

	return newlyAddedPair.first->second; // Returns a reference. Get the pair at pair.first, then the value at ->second
}

ResourceManager::shaderPointer ResourceManager::findShader(const std::string& shaderName) // Returns an lvalue reference, so you can modify it in the map. Make sure you keep it in the map though!
{
	// http://www.cplusplus.com/reference/unordered_map/unordered_map/find/
	shaderMap::iterator got = mShaderMap.find(shaderName); // Non-const iterator, since we want a non-const reference!

	if(got==mShaderMap.end())
	{
		std::string error = "Shader '" + shaderName + "' not found!";;
		Utils::crash(error, __LINE__, __FILE__);
		return got->second; // Can't return nothing here!
	}

	return got->second; // Dunno why you need ->
}

void ResourceManager::clearShaders() // For freeing memory, you don't have to call this when quitting
{
	mShaderMap.clear(); // Clears all shaders (if you want to know, calls all deconstructors)
}

ResourceManager::texturePointer ResourceManager::addTexture(const std::string& textureFile, const std::string& name, int type)
{
	std::string path = getFullResourcePath(textureFile);

	texturePointer texturePointer(new Texture(name, path, type));
	textureMapPair texturePair(name, texturePointer);

	std::pair<textureMap::iterator, bool> newlyAddedPair = mTextureMap.insert(texturePair); // Insert in map
	
	if(newlyAddedPair.second == false)
	{
		std::string error = "Texture '" + name + "' already exists and cannot be added again!";;
		Utils::crash(error, __LINE__, __FILE__);
		return newlyAddedPair.first->second; // Returns a reference to the texture that was there before
	}

	return newlyAddedPair.first->second;
}

// The texture will take the name of the texture file (characters before the first dot). This will make it easier to add many textures since you would probably name them the same anyway.
ResourceManager::texturePointer ResourceManager::addTexture(const std::string& textureFile, int type)
{
	std::string path = getFullResourcePath(textureFile);
	std::string name = getBasename(textureFile);

	return addTexture(textureFile, name, type); // Create the texture and return it
}

ResourceManager::texturePointer ResourceManager::findTexture(const std::string& textureName)
{
	textureMap::iterator got = mTextureMap.find(textureName);

	if(got == mTextureMap.end())
	{
		std::string error = "Texture '" + textureName + "' not found!";;
		Utils::crash(error, __LINE__, __FILE__);
		return got->second;
	}

	return got->second;
}

void ResourceManager::clearTextures()
{
	mTextureMap.clear();
}

// Objects are copied to make them easily modifiable. They are, however, stored as shared pointers for efficiency.
ObjectTemplate ResourceManager::addObjectTemplate(const std::string& objectFile, const std::string& name)
{
	std::string path = getFullResourcePath(objectFile);

	objectTemplatePointer objectTemplatePointer(new ObjectTemplate(name, path));
	objectTemplateMapPair objectTemplatePair(name, objectTemplatePointer);

	std::pair<objectTemplateMap::iterator, bool> newlyAddedPair = mObjectTemplateMap.insert(objectTemplatePair);
	
	if(newlyAddedPair.second == false)
	{
		std::string error = name;
		error = "Object template '" + error + "' already exists and cannot be added again!";
		Utils::crash(error, __LINE__, __FILE__);
		return *(newlyAddedPair.first->second); // Parenthesis for clarity
	}

	return *(newlyAddedPair.first->second);
}

ObjectTemplate ResourceManager::addObjectTemplate(const std::string& objectFile)
{
	std::string path = getFullResourcePath(objectFile);
	std::string name = getBasename(objectFile);

	return addObjectTemplate(objectFile, name);
}

ObjectTemplate ResourceManager::findObjectTemplate(const std::string& objectName) // Copies the object
{
	objectTemplateMap::iterator got = mObjectTemplateMap.find(objectName);

	if(got == mObjectTemplateMap.end()) // end() is past-the-end element iterator, so not found in the map!
	{
		std::string error = "Object template '" + objectName + "' not found!";;
		Utils::crash(error, __LINE__, __FILE__);
		return *got->second;
	}
	
	return *got->second;
}

void ResourceManager::clearObjectTemplates()
{
	mObjectTemplateMap.clear();
}