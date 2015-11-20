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

#ifndef RESOURCEMANAGER_HPP_
#define RESOURCEMANAGER_HPP_

#include <GLAD/glad.h>
#include <string>

#include <unordered_map>
#include <memory> // For shared_ptr
#include <Definitions.hpp>

#include <Shader.hpp>
#include <Texture.hpp>
#include <ObjectTemplate.hpp>

// All paths are prefixed with mResourceDir

class ResourceManager
{
private:
	// Each map will hold shared_ptrs to instances. When you remove this from the map, the instance will stay alive until all
	// shared_ptrs pointing to it are gone.
	typedef std::shared_ptr<Shader> shaderPointer;
	typedef std::unordered_map<std::string, shaderPointer> shaderMap; // Map of pointers
	typedef std::pair<std::string, shaderPointer> shaderMapPair; // These kinds of typedef are great inside the class definition like this

	typedef std::shared_ptr<Texture> texturePointer;
	typedef std::unordered_map<std::string, texturePointer> textureMap;
	typedef std::pair<std::string, texturePointer> textureMapPair;

	typedef std::shared_ptr<ObjectTemplate> objectTemplatePointer;
	typedef std::unordered_map<std::string, objectTemplatePointer> objectTemplateMap;
	typedef std::pair<std::string, objectTemplatePointer> objectTemplateMapPair;

	shaderMap mShaderMap; // Map, faster access: shaders[shaderName] = shaderID etc
	textureMap mTextureMap;
	objectTemplateMap mObjectTemplateMap;

	std::string mResourceDir;

	static std::string getBasename(const std::string& file);

public:
	ResourceManager(const std::string& resourceDir);
	~ResourceManager();

	std::string getFullResourcePath(const std::string& fileName);

	shaderPointer addShader(const std::string& shaderName, const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
	shaderPointer findShader(const std::string& shaderName);
	void clearShaders();

	texturePointer addTexture(const std::string& textureFile, const std::string& name, int type);
	texturePointer addTexture(const std::string& textureFile, int type);
	texturePointer findTexture(const std::string& textureName);
	void clearTextures();

	ObjectTemplate addObjectTemplate(const std::string& objectFile);
	ObjectTemplate addObjectTemplate(const std::string& objectFile, const std::string& name);
	ObjectTemplate findObjectTemplate(const std::string& objectName);
	void clearObjectTemplates();
};

#endif /* RESOURCEMANAGER_HPP_ */