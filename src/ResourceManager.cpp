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

#include "ResourceManager.hpp"
#include "Utils.hpp"

#include "glm/gtc/type_ptr.hpp"

#include <fstream>
#include <vector>
#include <cstddef> // For std::size_t
#include <fstream> // For object loading
#include <regex> // For searching in file paths and stuff

// Shaders must be ASCII or UTF-8
// Reference: http://duriansoftware.com/joe/An-intro-to-modern-OpenGL.-Chapter-2.2:-Shaders.html

// Resource names are also kept in their instances, so don't change them randomly without updating the resources

ResourceManager::ResourceManager(const std::string& basePath)
{
	mBasePath = basePath;
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
	// If the string is empty, don't bother!
	if(path.empty())
	{
		Utils::CRASH("Resource manager path is empy!");
		return std::string();
	}
	
	std::string file("");
	std::size_t lastPosition = path.find_last_of("/\\"); // Search for the last slash or backslash (escaped here)
	
	if(lastPosition != std::string::npos) // Slash found
	{
		std::size_t firstFileNameChar = lastPosition + 1;

		if(firstFileNameChar > path.length()) // There are no characters after the slash
		{
			Utils::CRASH("Resource manager path '" + path + "' is a directory, not a file!");
			return std::string();
		}

		file = path.substr(firstFileNameChar);
	} else
	{
		// No slash in the path
		file = path;
	}
		

	// Get the basename, so remove the extension if it was present
	std::size_t firstDot = file.find('.'); // Returns the index of the first found dot

	if(firstDot == std::string::npos) // No dot in this file
		return file; // Set the name as the whole file name
	else
		return file.substr(0, firstDot); // The index of the first dot aka the length of the name
}

// Returns the full absolute resource path
// Example: level1/fun.obj -> C:/Program Files/SDL3D/resources/level1/fun.obj
// This makes sure it will on most platforms and if the game is being launched from somewhere else
std::string ResourceManager::getFullResourcePath(const std::string& path)
{
	return mBasePath + RESOURCE_PATH_PREFIX + path;
}

std::string ResourceManager::getFullShaderPath(const std::string& path)
{
	return getFullResourcePath(SHADER_PATH_PREFIX + path);
}

// Example: main.lua -> C:/Program Files/SDL3D/resources/scripts/main.lua
std::string ResourceManager::getFullScriptPath(const std::string& path)
{
	return getFullResourcePath(SCRIPT_PATH_PREFIX + path);
}

/////// Shaders ///////
// Factory
// I find that 'add' is a good verb since ResourceManager will TRACK the shader, but my opinion is subject to change.
ResourceManager::shaderPointer
	ResourceManager::addShader(const std::string& name, const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
	std::string vertexShaderPath = getFullShaderPath(vertexShaderFile); // All resources are in the resource dir
	std::string fragmentShaderPath = getFullShaderPath(fragmentShaderFile);

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

ResourceManager::shaderPointer
	ResourceManager::addShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
	std::string name = getBasename(vertexShaderFile); // Get the basename of one file, implying they are the same on both
	return addShader(name, vertexShaderFile, fragmentShaderFile);
}

// Returns a smart pointer, so you can use it wherever you want however you want and it will never be invalid
// It is non-const like this you can modify it outside of objects
// It returns a smart pointer instead of a reference since the objects store smart pointers
ResourceManager::shaderPointer ResourceManager::findShader(const std::string& name)
{
	// http://www.cplusplus.com/reference/map/map/find/
	shaderMap::iterator got = mShaderMap.find(name); // Non-const iterator, since we want a non-const reference!

	if(got==mShaderMap.end())
	{
		std::string error = "Shader '" + name + "' cannot be found! Did you add it?";
		Utils::CRASH(error);
		return got->second; // Can't return nothing here!
	}

	return got->second; // Dunno why you need ->
}

void ResourceManager::clearShaders() // For freeing memory, you don't have to call this when quitting
{
	mShaderMap.clear(); // Clears all shaders (if you want to know, calls all deconstructors)
}

/////// Textures ///////
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

ResourceManager::texturePointer ResourceManager::findTexture(const std::string& name)
{
	textureMap::iterator got = mTextureMap.find(name);

	if(got == mTextureMap.end())
	{
		std::string error = "Texture '" + name + "' cannot be found! Did you add it?";
		Utils::CRASH(error);
		return got->second;
	}

	return got->second;
}

void ResourceManager::clearTextures()
{
	mTextureMap.clear();
}


/////// ObjectGeometryGroups ///////
ResourceManager::objectGeometryGroup_pointer
	ResourceManager::addObjectGeometryGroup(const std::string& name, const std::string& objectFile)
{
	std::string path = getFullResourcePath(objectFile);
	objectGeometryGroup_pointer group(new ObjectGeometryGroup(name, path));

	return addObjectGeometryGroup(group);
}

ResourceManager::objectGeometryGroup_pointer
	ResourceManager::addObjectGeometryGroup(const std::string& objectFile)
{
	std::string name = getBasename(objectFile);
	return addObjectGeometryGroup(name, objectFile);
}

// Gets the name from the object geometry group pointer
// This function lets us use Resourcemanager to keep track of custom-made object geometry groups
ResourceManager::objectGeometryGroup_pointer
	ResourceManager::addObjectGeometryGroup(objectGeometryGroup_pointer objectGeometryGroupPointer)
{
	std::string name = objectGeometryGroupPointer->getName();

	objectGeometryGroup_mapPair groupPair(name, objectGeometryGroupPointer);
	std::pair<objectGeometryGroup_map::iterator, bool> newlyAddedPair = mObjectGeometryGroupMap.insert(groupPair);

	if(newlyAddedPair.second == false) // It already exists in the map
	{
		std::string error = "Object geometry group '" + name + "' already exists and cannot be added!";
		Utils::CRASH(error);
		return newlyAddedPair.first->second;
	}

	return newlyAddedPair.first->second;
}

ResourceManager::objectGeometryGroup_pointer
	ResourceManager::findObjectGeometryGroup(const std::string& name) // Gives a pointer
{
	objectGeometryGroup_map::iterator got = mObjectGeometryGroupMap.find(name);

	if(got == mObjectGeometryGroupMap.end()) // end() is past-the-end element iterator, so not found in the map!
	{
		std::string error = "Object geometry group '" + name + "' cannot be found! Did you add it?";
		Utils::CRASH(error);
		return got->second;
	}
	
	return got->second;
}

void ResourceManager::clearObjectGeometryGroups()
{
	mObjectGeometryGroupMap.clear();
}

/////// Scripts ///////
ResourceManager::scriptPointer ResourceManager::addScript(const std::string& name, const std::string& mainScriptFile)
{
	std::string mainFilePath = getFullScriptPath(mainScriptFile);

	// getFullScriptPath("") will return a good require directory, logically
	scriptPointer script(new Script(name, mainFilePath, getFullScriptPath("")));

	scriptMapPair scriptPair(name, script);
	std::pair<scriptMap::iterator, bool> newlyAddedPair = mScriptMap.insert(scriptPair);

	if(newlyAddedPair.second == false) // It already exists in the map
	{
		std::string error = "Script '" + name + "' already exists and cannot be added again!";
		Utils::CRASH(error);
		return newlyAddedPair.first->second;
	}

	return newlyAddedPair.first->second; // Get the pair at pair.first, then the pointer at ->second
}

ResourceManager::scriptPointer ResourceManager::addScript(const std::string& mainScriptFile)
{
	std::string name = getBasename(mainScriptFile);
	return addScript(name, mainScriptFile);
}

ResourceManager::scriptPointer ResourceManager::findScript(const std::string& name)
{
	scriptMap::iterator got = mScriptMap.find(name);

	if(got == mScriptMap.end())
	{
		std::string error = "Script '" + name + "' cannot be found! Did you add it?";
		Utils::CRASH(error);
		return got->second;
	}

	return got->second;
}

void ResourceManager::clearScripts()
{
	mScriptMap.clear();
}

/////// Sounds ///////
ResourceManager::soundPointer ResourceManager::addSound(const std::string& name, const std::string& soundFile, int type)
{
	std::string soundFilePath = getFullResourcePath(soundFile);

	soundPointer sound(new Sound(name, soundFilePath, type));

	soundMapPair soundPair(name, sound);
	std::pair<soundMap::iterator, bool> newlyAddedPair = mSoundMap.insert(soundPair);

	if(newlyAddedPair.second == false) // It already exists in the map
	{
		std::string error = "Sound '" + name + "' already exists and cannot be added again!";
		Utils::CRASH(error);
		return newlyAddedPair.first->second;
	}

	return newlyAddedPair.first->second; // Get the pair at pair.first, then the pointer at ->second
}

ResourceManager::soundPointer ResourceManager::addSound(const std::string& soundFile, int type)
{
	std::string name = getBasename(soundFile);
	return addSound(name, soundFile, type);
}

ResourceManager::soundPointer ResourceManager::findSound(const std::string& name)
{
	soundMap::iterator got = mSoundMap.find(name);

	if(got == mSoundMap.end())
	{
		std::string error = "Sound '" + name + "' cannot be found! Did you add it?";
		Utils::CRASH(error);
		return got->second;
	}

	return got->second;
}

void ResourceManager::clearSounds()
{
	mSoundMap.clear();
}
