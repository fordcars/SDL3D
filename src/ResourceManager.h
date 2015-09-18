#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <GLAD/glad.h>
#include <string>

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

#include <unordered_map>
#include <memory> // For shared_ptr
#include <Definitions.h>

#include <Shader.h>
#include <Texture.h>

// All paths are prefixed with mResourceDir

class ResourceManager
{

private:
	typedef std::shared_ptr<Shader> shaderPointer;
	typedef std::shared_ptr<Texture> texturePointer;

	// Each map will hold shared_ptrs to instances. When you remove this from the map, the instance will stay alive until all
	// shared_ptrs pointing to it are gone.
	typedef std::unordered_map<std::string, shaderPointer> shaderMap; // Map of pointers
	typedef std::pair<std::string, shaderPointer> shaderMapPair; // These kinds of typedef are great inside the class definition like this

	typedef std::unordered_map<std::string, texturePointer> textureMap;
	typedef std::pair<std::string, texturePointer> textureMapPair;
	shaderMap mShaders; // Map, faster access: shaders[shaderName] = shaderID etc
	textureMap mTextures;

	std::string mResourceDir;

public:
	ResourceManager(const std::string& resourceDir);
	~ResourceManager();

	static std::string getFileContents(const std::string& fileName); // Static functions: no need for an instance to use them!
	
	std::string getFullResourcePath(const std::string& fileName);

	shaderPointer addShader(const std::string& shaderName, const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
	shaderPointer findShader(const std::string& shaderName);
	void clearShaders();

	texturePointer addTexture(const std::string& name, const std::string& textureFile, int type);
	texturePointer addTexture(const std::string& textureFile, int type);
	texturePointer findTexture(const std::string& textureName);
	void clearTextures();
};

#endif /* RESOURCEMANAGER_H_ */