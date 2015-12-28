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
#include <tiny_obj_loader.h>

#include <ResourceManager.hpp>
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

	shaderPointer shader(new Shader(shaderName, vertexShaderPath, fragmentShaderPath)); // Create a smart pointer of a shader instance

	shaderMapPair shaderPair(shaderName, shader);
	std::pair<shaderMap::iterator, bool> newlyAddedPair = mShaderMap.insert(shaderPair);
	
	if(newlyAddedPair.second == false) // It already exists in the map
	{
		std::string error = "Shader '" + shaderName + "' already exists and cannot be added again!";;
		Utils::CRASH(error);
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
		Utils::CRASH(error);
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

	texturePointer texture(new Texture(name, path, type));
	textureMapPair texturePair(name, texture);

	std::pair<textureMap::iterator, bool> newlyAddedPair = mTextureMap.insert(texturePair); // Insert in map
	
	if(newlyAddedPair.second == false)
	{
		std::string error = "Texture '" + name + "' already exists and cannot be added again!";;
		Utils::CRASH(error);
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
		Utils::CRASH(error);
		return got->second;
	}

	return got->second;
}

void ResourceManager::clearTextures()
{
	mTextureMap.clear();
}

/*// Object geometries are copied to make them easily modifiable. They are, however, stored as shared pointers for efficiency.
void ResourceManager::addObjectGeometries(const std::string& objectFile)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string error;

	tinyobj::LoadObj(shapes, materials, error, objectFile.c_str());

	if(!error.empty())
	{
		Utils::LOGPRINT(".obj file '" + objectFile + "' failed to load!");
		Utils::CRASH("Tinyobjloader error message: " + error);

}*/

ObjectGeometry ResourceManager::addObjectGeometry(const std::string& objectFile, const std::string& name)
{
	std::string path = getFullResourcePath(objectFile);

	objectGeometryPointer objectGeometry(new ObjectGeometry(name, path));
	objectGeometryMapPair objectGeometryPair(name, objectGeometry);

	std::pair<objectGeometryMap::iterator, bool> newlyAddedPair = mObjectGeometryMap.insert(objectGeometryPair);
	
	if(newlyAddedPair.second == false)
	{
		std::string error = name;
		error = "Object geometry '" + error + "' already exists and cannot be added again!";
		Utils::CRASH(error);
		return *(newlyAddedPair.first->second); // Parenthesis for clarity
	}

	return *(newlyAddedPair.first->second);
}

ObjectGeometry ResourceManager::addObjectGeometry(const std::string& objectFile)
{
	std::string path = getFullResourcePath(objectFile);
	std::string name = getBasename(objectFile);

	return addObjectGeometry(objectFile, name);
}

ObjectGeometry ResourceManager::findObjectGeometry(const std::string& objectName) // Copies the geometry
{
	objectGeometryMap::iterator got = mObjectGeometryMap.find(objectName);

	if(got == mObjectGeometryMap.end()) // end() is past-the-end element iterator, so not found in the map!
	{
		std::string error = "Object geometry '" + objectName + "' not found!";;
		Utils::CRASH(error);
		return *got->second;
	}
	
	return *got->second;
}

void ResourceManager::clearObjectGeometries()
{
	mObjectGeometryMap.clear();
}