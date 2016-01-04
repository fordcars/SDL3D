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

#include <fstream>
#include <vector>
#include <cstdlib> // For std::size_t
#include <fstream> // For object loading
#include <regex>

#include <glm/gtc/type_ptr.hpp>

#include <ResourceManager.hpp>
#include <Utils.hpp>

// Shaders must be ASCII or UTF-8
// Reference: http://duriansoftware.com/joe/An-intro-to-modern-OpenGL.-Chapter-2.2:-Shaders.html

// Resource names are also kept in their instances, so don't change them randomly without updating the resources

ResourceManager::ResourceManager(const std::string& resourceDir)
{
	mResourceDir = resourceDir;
}

ResourceManager::~ResourceManager()
{
	clearShaders();
}

// Static
// Get the name of the file before the first dot '.'. Works with paths or files.
// Example: C:\Users\File.dll -> File
std::string ResourceManager::getBasename(const std::string& path)
{
	// IF the string is empty, don't bother!
	if(path.empty())
		return std::string();

	// Get the file out of the path, example: thing.dll
	// http://stackoverflow.com/questions/4154070/getting-just-the-file-name-from-full-path
	std::smatch match;
	std::regex regex("^.*[\\/\\\\]"); // C++ and character escaping
	bool isPath = std::regex_search(path, match, regex); // If this is false, it is simply a file

	std::string file = "";

	if(isPath)
		file = match.suffix().str();
	else
		// If it is not a path, it was already a file (or lets hope so)
		file = path;

	Utils::LOGPRINT(file);

	// Get the basename, example: thing
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
ResourceManager::shaderPointer ResourceManager::addShader(const std::string& name, const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
	std::string vertexShaderPath = getFullResourcePath(vertexShaderFile); // All resources are in the resource dir
	std::string fragmentShaderPath = getFullResourcePath(fragmentShaderFile);

	shaderPointer shader(new Shader(name, vertexShaderPath, fragmentShaderPath)); // Create a smart pointer of a shader instance

	shaderMapPair shaderPair(name, shader);
	std::pair<shaderMap::iterator, bool> newlyAddedPair = mShaderMap.insert(shaderPair);
	
	if(newlyAddedPair.second == false) // It already exists in the map
	{
		std::string error = "Shader '" + name + "' already exists and cannot be added again!";
		Utils::CRASH(error);
		return newlyAddedPair.first->second; // Returns a pointer to the shader that was there before
	}

	return newlyAddedPair.first->second; // Get the pair at pair.first, then the pointer at ->second
}

// Returns a smart pointer, so you can use it wherever you want however you want and it will never be invalid
// It is non-const like this you can modify it outside of objects
// It returns a smart pointer instead of a reference since the objects store smart pointers
ResourceManager::shaderPointer ResourceManager::findShader(const std::string& name)
{
	// http://www.cplusplus.com/reference/unordered_map/unordered_map/find/
	shaderMap::iterator got = mShaderMap.find(name); // Non-const iterator, since we want a non-const reference!

	if(got==mShaderMap.end())
	{
		std::string error = "Shader '" + name + "' not found!";
		Utils::CRASH(error);
		return got->second; // Can't return nothing here!
	}

	return got->second; // Dunno why you need ->
}

void ResourceManager::clearShaders() // For freeing memory, you don't have to call this when quitting
{
	mShaderMap.clear(); // Clears all shaders (if you want to know, calls all deconstructors)
}

ResourceManager::texturePointer ResourceManager::addTexture(const std::string& name, const std::string& textureFile, int type)
{
	std::string path = getFullResourcePath(textureFile);

	texturePointer texture(new Texture(name, path, type));
	textureMapPair texturePair(name, texture);

	std::pair<textureMap::iterator, bool> newlyAddedPair = mTextureMap.insert(texturePair); // Insert in map
	
	if(newlyAddedPair.second == false)
	{
		std::string error = "Texture '" + name + "' already exists and cannot be added again!";
		Utils::CRASH(error);
		return newlyAddedPair.first->second; // Returns a pointer to the texture that was there before
	}

	return newlyAddedPair.first->second;
}

// The texture will take the name of the texture file (characters before the first dot). This will make it easier to add many textures since you would probably name them the same anyway.
ResourceManager::texturePointer ResourceManager::addTexture(const std::string& textureFile, int type)
{
	std::string name = getBasename(textureFile);
	return addTexture(name, textureFile, type); // Create the texture and return it
}

ResourceManager::texturePointer ResourceManager::findTexture(const std::string& textureName)
{
	textureMap::iterator got = mTextureMap.find(textureName);

	if(got == mTextureMap.end())
	{
		std::string error = "Texture '" + textureName + "' not found!";
		Utils::CRASH(error);
		return got->second;
	}

	return got->second;
}

void ResourceManager::clearTextures()
{
	mTextureMap.clear();
}

ResourceManager::objectGeometryGroup_pointer
	ResourceManager::addObjectGeometryGroup(const std::string& name, const std::string& objectFile, bool splitGeometries)
{
	std::string path = getFullResourcePath(objectFile);
	objectGeometryGroup_pointer group(new ObjectGeometryGroup(name, path, splitGeometries));

	objectGeometryGroup_mapPair groupPair(name, group);
	std::pair<objectGeometryGroup_map::iterator, bool> newlyAddedPair = mObjectGeometryGroupMap.insert(groupPair);
	
	if(newlyAddedPair.second == false) // It already exists in the map
	{
		std::string error = "Object geometry group '" + name + "' already exists!";
		Utils::CRASH(error);
		return newlyAddedPair.first->second;
	}

	return newlyAddedPair.first->second;
}

ResourceManager::objectGeometryGroup_pointer
	ResourceManager::addObjectGeometryGroup(const std::string& objectFile, bool splitGeometries)
{
	std::string name = getBasename(objectFile);

	return addObjectGeometryGroup(name, objectFile, splitGeometries); // Create the texture and return it
}

ResourceManager::objectGeometryGroup_pointer
	ResourceManager::findObjectGeometryGroup(const std::string& name) // Gives a pointer
{
	objectGeometryGroup_map::iterator got = mObjectGeometryGroupMap.find(name);

	if(got == mObjectGeometryGroupMap.end()) // end() is past-the-end element iterator, so not found in the map!
	{
		std::string error = "Object geometry group '" + name + "' not found!";
		Utils::CRASH(error);
		return got->second;
	}
	
	return got->second;
}

void ResourceManager::clearObjectGeometries()
{
	mObjectGeometryGroupMap.clear();
}