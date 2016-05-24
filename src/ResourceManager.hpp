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

#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include "Shader.hpp"
#include "Texture.hpp"
#include "ObjectGeometryGroup.hpp"
#include "Script.hpp"
#include "Sound.hpp"

#include "Definitions.hpp"

#include "glad/glad.h"

#include <string>
#include <map>
#include <memory> // For shared_ptr

// All paths are prefixed with mResourceDir

class Game;
class ResourceManager
{
public:
	// Since these values are returned
	using shaderPointer                 = std::shared_ptr<Shader>;
	using texturePointer                = std::shared_ptr<Texture>;
	using objectGeometryGroup_pointer   = std::shared_ptr<ObjectGeometryGroup>; // Underscore for clarity
	using scriptPointer                 = std::shared_ptr<Script>;
	using soundPointer                  = std::shared_ptr<Sound>;

private:
	// Each map will hold shared_ptrs to instances. When you remove this from the map, the instance will stay alive until all
	// shared_ptrs pointing to it are gone.
	using shaderMap     = std::map<std::string, shaderPointer>; // Map of pointers
	using shaderMapPair = std::pair<std::string, shaderPointer>; // These aliases are great inside the class definition like this

	using textureMap     = std::map<std::string, texturePointer>;
	using textureMapPair = std::pair<std::string, texturePointer>;

	using objectGeometryGroup_map     = std::map<std::string, objectGeometryGroup_pointer>;
	using objectGeometryGroup_mapPair = std::pair<std::string, objectGeometryGroup_pointer>;

	using scriptMap     = std::map<std::string, scriptPointer>;
	using scriptMapPair = std::pair<std::string, scriptPointer>;

	using soundMap     = std::map<std::string, soundPointer>;
	using soundMapPair = std::pair<std::string, soundPointer>;

	Game& mGame; // Keep a reference to the game, for easy communication between components

	shaderMap mShaderMap; // Map, faster access: shaders[shaderName] = shaderID etc
	textureMap mTextureMap;
	objectGeometryGroup_map mObjectGeometryGroupMap;
	scriptMap mScriptMap;
	soundMap mSoundMap;

	std::string mBasePath; // This is directory the game is in or, in a Mac bundle, the bundle's Resources directory. Absolute path.

public:
	ResourceManager(Game& game);
	ResourceManager(Game& game, const std::string& basePath);
	~ResourceManager();

	static std::string getBasename(const std::string& path);

	void setBasePath(const std::string& basePath);
	std::string getFullResourcePath(const std::string& path);
	std::string getFullShaderPath(const std::string& path);
	std::string getFullScriptPath(const std::string& path);

	// Factories
	shaderPointer addShader(const std::string& name, const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
	shaderPointer addShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
	shaderPointer findShader(const std::string& name);
	void clearShaders();

	texturePointer addTexture(const std::string& name, const std::string& textureFile, int type);
	texturePointer addTexture(const std::string& textureFile, int type);
	texturePointer findTexture(const std::string& name);
	void clearTextures();

	objectGeometryGroup_pointer addObjectGeometryGroup(const std::string& name, const std::string& objectFile);
	objectGeometryGroup_pointer addObjectGeometryGroup(const std::string& objectFile);
	objectGeometryGroup_pointer addObjectGeometryGroup(objectGeometryGroup_pointer objectGeometryGroupPointer);
	objectGeometryGroup_pointer findObjectGeometryGroup(const std::string& objectName);
	void clearObjectGeometryGroups();

	scriptPointer addScript(const std::string& name, const std::string& mainScriptFile);
	scriptPointer addScript(const std::string& mainScriptFile);
	scriptPointer findScript(const std::string& name);
	void clearScripts();

	soundPointer addSound(const std::string& name, const std::string& soundFile, int type);
	soundPointer addSound(const std::string& soundFile, int type);
	soundPointer findSound(const std::string& name);
	void clearSounds();
};

#endif // RESOURCE_MANAGER_HPP
