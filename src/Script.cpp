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

// The whole interface is defined here! This class has A LOT of dependencies!

// This class is a single script. A script can be multiple files, but this is Lua's job.
// Happily, it looks like splitting the lua states per script, like here, is a good idea.
// http://lua-users.org/lists/lua-l/2003-11/msg00011.html

#include <Script.hpp>
#include <Utils.hpp>
#include <Game.hpp>

#include <exception> // For handling the script's exceptions
#include <memory> // For smart pointers
#include <vector>

using namespace LuaIntf; // Will make things way clearer

// mainFilePath is the script's entry point file
Script::Script(const std::string& name, const std::string& mainFilePath, const std::string& absoluteRequirePath)
{
	mName = name;
	mMainFilePath = mainFilePath;
	mMainFileContents = Utils::getFileContents(mainFilePath);
	setLuaRequirePath(absoluteRequirePath);

	try
	{
		LuaState luaState = mLuaContext.state();
		luaState.openLibs(); // Open all of Lua's basic libs, like io and math.
	} catch(const std::exception& e)
	{
		std::string errorMessage = "Lua failed to load libraries. Error: ";
		Utils::CRASH(errorMessage + e.what());
	}
}

Script::~Script()
{
	// Do nothing
}

// Will logprint a clarified error message and it's solutions if it was a know error
// Returns true if there was a know error that was clarified
bool Script::clarifyError(const std::string& errorMessage)
{
	// The error messages found here were found by trial and error
	// These are only guesses and might not be 100% acurate
	bool clarifiedError = false; // True if we clarified an error

	// A function is returning an object that wasn't binded!
	if(errorMessage.find("bad argument #-2") != std::string::npos)
	{
		clarifiedError = true;
		Utils::WARN("We predict a function is returning a non-binded object. If this is the case, this is a bug in this engine! Please contact the developpers.");
	}

	// A module fonction is being called using the ':' syntax instead of the '.' syntax!
	if(errorMessage.find("on bad self") != std::string::npos)
	{
		clarifiedError = true;
		Utils::WARN("We predict you are attempting to call a module function using the ':'. Please use the '.' syntax instead!");
	}

	return clarifiedError;
}

// Lua will search in this path when using require().
// Lua requires an absolute path!
bool Script::setLuaRequirePath(const std::string& absolutePath)
{
	try
	{
		LuaState luaState = mLuaContext.state();

		// From http://stackoverflow.com/questions/4125971/setting-the-global-lua-path-variable-from-c-c/4156038#4156038
		luaState.getGlobal("package");
		luaState.getField(-1, "path");
		
		std::string currentPath = luaState.toString(-1);
		currentPath.append(";");
		currentPath.append(absolutePath + "?.lua"); // ?.lua seems to be needed and works better for cross-platform

		luaState.pop(1);
		luaState.push(currentPath.c_str());
		luaState.setField(-2, "path");
		luaState.pop(1);
	} catch(const std::exception& e)
	{
		std::string errorMessage = "Failed to set Lua's require path for script '" + mName + "'! Error: ";
		Utils::CRASH(errorMessage + e.what());
		return false;
	}

	return true;
}

// All includes for the bindings
#include <Game.hpp>
#include <ResourceManager.hpp>
#include <InputManager.hpp>
#include <EntityManager.hpp>

#include <Shader.hpp>
#include <Texture.hpp>
#include <ObjectGeometryGroup.hpp>
#include <ObjectGeometry.hpp>
#include <Sound.hpp>
#include <GPUBuffer.hpp>
#include <Entity.hpp>
#include <Camera.hpp>

#include <Object.hpp>
#include <TexturedObject.hpp>
#include <ShadedObject.hpp>

#include <Utils.hpp>

#include <Definitions.hpp>
#include <glm/glm.hpp>
#include <SDL_keycode.h> // For key codes

// Binds all of the classes and functions. This creates our API!
// We need a Game instance to give it to the scripts.
void Script::bindInterface(Game& game)
{
	// Notes:
	// If you get an 'arg #-2 is nil' error, it is propably because the failing function is returning an object that is not binded!
	// You cannot give Lua different functions with the same name (overloaded Lua side). You have to give them different names (only on the Lua side).
	// Use a static_Cast to get the right overload.
	// You need to add a constructor to construct an object on the Lua side
	// You need to specify args with LUA_ARGS when binding a constructor with LUA_SP container (shared_ptr)
	// When you get a 'calling on bad self' error from a module function call, make sure that you are calling this module fonction with the '.' sintax! (Not ':')

	LuaState luaState = mLuaContext.state();

	LuaBinding(luaState).addFunction("getGame", [&game]() -> Game& // The -> specifies the return type. The () are needed for this syntax.
	{
		return game;
	});

	LuaBinding(luaState).beginClass<Game>("Game")
		.addFunction("quit", &Game::quit) // &Game::quit returns quit()'s address
		.addFunction("setName", &Game::setName)
		.addFunction("setSize", &Game::setSize)
		.addFunction("setMaxFramesPerSecond", &Game::setMaxFramesPerSecond)
		.addFunction("setMainWindowPosition", &Game::setMainWindowPosition)
		.addFunction("reCenterMainWindow", &Game::reCenterMainWindow)

		.addFunction("getResourceManager", &Game::getResourceManager)
		.addFunction("getInputManager", &Game::getInputManager)
		.addFunction("getEntityManager", &Game::getEntityManager)
	.endClass();


	LuaBinding(luaState).beginClass<ResourceManager>("ResourceManager")
		.addFunction("addShader",
			// Specify which overload we want. Lua doesn't support functions with same names, though.
			static_cast<ResourceManager::shaderPointer(ResourceManager::*)(const std::string&, const std::string&)>
				(&ResourceManager::addShader))

		.addFunction("addNamedShader",
			static_cast<ResourceManager::shaderPointer(ResourceManager::*)(const std::string&, const std::string&, const std::string&)>
				(&ResourceManager::addShader))

		.addFunction("findShader", &ResourceManager::findShader)
		.addFunction("clearShaders", &ResourceManager::clearShaders)

		.addFunction("addTexture",
			static_cast<ResourceManager::texturePointer(ResourceManager::*)(const std::string&, int)>
				(&ResourceManager::addTexture))

		.addFunction("addNamedTexture",
			static_cast<ResourceManager::texturePointer(ResourceManager::*)(const std::string&, const std::string&, int)>
				(&ResourceManager::addTexture))

		.addFunction("findTexture", &ResourceManager::findTexture)
		.addFunction("clearTextures", &ResourceManager::clearTextures)

		.addFunction("addObjectGeometryGroup",
			static_cast<ResourceManager::objectGeometryGroup_pointer(ResourceManager::*)(const std::string&)>
			(&ResourceManager::addObjectGeometryGroup))

		.addFunction("addNamedObjectGeometryGroup",
			static_cast<ResourceManager::objectGeometryGroup_pointer(ResourceManager::*)(const std::string&, const std::string&)>
			(&ResourceManager::addObjectGeometryGroup))

		.addFunction("findObjectGeometryGroup", &ResourceManager::findObjectGeometryGroup)
		.addFunction("clearObjectGeometryGroups", &ResourceManager::clearObjectGeometryGroups)

		.addFunction("addSound",
			static_cast<ResourceManager::soundPointer(ResourceManager::*)(const std::string&, int)>
			(&ResourceManager::addSound))

		.addFunction("addNamedSound",
			static_cast<ResourceManager::soundPointer(ResourceManager::*)(const std::string&, const std::string&, int)>
			(&ResourceManager::addSound))

		.addFunction("findSound", &ResourceManager::findSound)
		.addFunction("clearSounds", &ResourceManager::clearSounds)
	.endClass();


	LuaBinding(luaState).beginClass<Shader>("Shader")
		.addFunction("getName", &Shader::getName)
	.endClass();


	LuaBinding(luaState).beginClass<Texture>("Texture")
		.addFunction("getName", &Texture::getName)
		.addFunction("getType", &Texture::getType)
	.endClass();


	LuaBinding(luaState).beginModule("TextureType")
		.addConstant("BMP", TEXTURE_BMP)
		.addConstant("DDS", TEXTURE_DDS)
	.endModule();


	LuaBinding(luaState).beginClass<ObjectGeometryGroup>("ObjectGeometryGroup")
		.addFunction("getName", &ObjectGeometryGroup::getName)
		.addFunction("addObjectGeometry", &ObjectGeometryGroup::addObjectGeometry)
		.addFunction("findObjectGeometry", &ObjectGeometryGroup::findObjectGeometry)
		.addFunction("getObjectGeometries", &ObjectGeometryGroup::findObjectGeometry)
	.endClass();


	LuaBinding(luaState).beginClass<ObjectGeometry>("ObjectGeometry")
		// You can create this type of object, but it will be stored as an std::shared_ptr
		// You need to specify LUA_ARGS with a LUA_SP container
		.addConstructor(LUA_SP(std::shared_ptr<ObjectGeometry>), LUA_ARGS(
			const std::string&,
			const ObjectGeometry::uintVector&,
			const ObjectGeometry::vec3Vector&,
			const ObjectGeometry::vec2Vector&,
			const ObjectGeometry::vec3Vector&))

		.addFunction("getName", &ObjectGeometry::getName)
	.endClass();


	LuaBinding(luaState).beginClass<Sound>("Sound")
		.addFunction("getName", &Sound::getName)
		.addFunction("play", &Sound::play, LUA_ARGS(_def<int, 0>))
		.addFunction("isPlaying", &Sound::isPlaying)
		.addFunction("halt", &Sound::halt)
		.addFunction("pause", &Sound::pause)
		.addFunction("isPaused", &Sound::isPaused)
		.addFunction("resume", &Sound::resume)

		.addFunction("fadeIn", &Sound::fadeIn, LUA_ARGS(int, _def<int, 0>))
		.addFunction("fadeOut", &Sound::fadeOut)

		.addFunction("setVolume", &Sound::setVolume)
		.addFunction("getVolume", &Sound::getVolume)
	.endClass();


	LuaBinding(luaState).beginModule("SoundType")
		.addConstant("Music", SOUND_MUSIC)
		.addConstant("Chunk", SOUND_CHUNK)
	.endModule();


	// Bind a few useful GPUBuffers
	LuaBinding(luaState).beginClass<ObjectGeometry::uintBuffer>("GPUBuffer_uint>")
		.addFunction("setMutableData", &ObjectGeometry::uintBuffer::setMutableData)
		.addFunction("setImmutableData", &ObjectGeometry::uintBuffer::setImmutableData)

		// No argument version of readData()
		.addFunction("readData",
			static_cast<std::vector<unsigned int>(ObjectGeometry::uintBuffer::*)() const> (&ObjectGeometry::uintBuffer::readData))
	.endClass();


	LuaBinding(luaState).beginClass<ObjectGeometry::vec2Buffer>("GPUBuffer_vec2>")
		.addFunction("setMutableData", &ObjectGeometry::vec2Buffer::setMutableData)
		.addFunction("setImmutableData", &ObjectGeometry::vec2Buffer::setImmutableData)

		// No argument version of readData()
		.addFunction("readData",
			static_cast<std::vector<glm::vec2>(ObjectGeometry::vec2Buffer::*)() const> (&ObjectGeometry::vec2Buffer::readData))
	.endClass();


	LuaBinding(luaState).beginClass<ObjectGeometry::vec3Buffer>("GPUBuffer_vec3>")
		.addFunction("setMutableData", &ObjectGeometry::vec3Buffer::setMutableData)
		.addFunction("setImmutableData", &ObjectGeometry::vec3Buffer::setImmutableData)

		// No argument version of readData()
		.addFunction("readData",
			static_cast<std::vector<glm::vec3>(ObjectGeometry::vec3Buffer::*)() const> (&ObjectGeometry::vec3Buffer::readData))
	.endClass();


	LuaBinding(luaState).beginClass<InputManager>("InputManager")
		.addFunction("registerKey", &InputManager::registerKey)
		.addFunction("registerKeys", &InputManager::registerKeys)
		.addFunction("isKeyPressed", &InputManager::isKeyPressed)
	.endClass();


	// Bind SDL key codes (not all of them, we are lazy)
	LuaBinding(luaState).beginModule("KeyCode")
		.addConstant("UP", SDLK_UP)
		.addConstant("DOWN", SDLK_DOWN)
		.addConstant("LEFT", SDLK_LEFT)
		.addConstant("RIGHT", SDLK_RIGHT)
		.addConstant("LSHIFT", SDLK_LSHIFT)
		.addConstant("RSHIFT", SDLK_RSHIFT)
		.addConstant("LCTRL", SDLK_LCTRL)
		.addConstant("RCTRL", SDLK_RCTRL)
		.addConstant("SPACE", SDLK_SPACE)
		.addConstant("BACKSPACE", SDLK_BACKSPACE)
		.addConstant("a", SDLK_a)
		.addConstant("b", SDLK_b)
		.addConstant("c", SDLK_c)
		.addConstant("d", SDLK_d)
		.addConstant("e", SDLK_e)
		.addConstant("f", SDLK_f)
		.addConstant("g", SDLK_g)
		.addConstant("h", SDLK_h)
		.addConstant("i", SDLK_i)
		.addConstant("j", SDLK_j)
		.addConstant("k", SDLK_k)
		.addConstant("l", SDLK_l)
		.addConstant("m", SDLK_m)
		.addConstant("n", SDLK_n)
		.addConstant("o", SDLK_o)
		.addConstant("p", SDLK_p)
		.addConstant("q", SDLK_q)
		.addConstant("r", SDLK_r)
		.addConstant("s", SDLK_s)
		.addConstant("t", SDLK_t)
		.addConstant("u", SDLK_u)
		.addConstant("v", SDLK_v)
		.addConstant("w", SDLK_w)
		.addConstant("x", SDLK_x)
		.addConstant("y", SDLK_y)
		.addConstant("z", SDLK_z)
	.endModule();


	LuaBinding(luaState).beginClass<EntityManager>("EntityManager")
		.addFunction("getGameCamera", &EntityManager::getGameCamera)
		.addFunction("addObject", &EntityManager::addObject)
		.addFunction("getObjects", &EntityManager::getObjects)
		.addFunction("addLight", &EntityManager::addLight)
		.addFunction("getLights", &EntityManager::getLights)
		.endClass();


	LuaBinding(luaState).beginClass<Entity>("Entity")
		.addFunction("setPosition", &Entity::setPosition)
		.addFunction("getPosition", &Entity::getPosition)

		.addFunction("setScaling", &Entity::setScaling)
		.addFunction("getScaling", &Entity::getScaling)

		.addFunction("setRotation", &Entity::setRotation)
		.addFunction("getRotation", &Entity::getRotation)

		.addFunction("setVelocity", &Entity::setVelocity)
		.addFunction("getVelocity", &Entity::getVelocity)
		.endClass();


	LuaBinding(luaState).beginExtendClass<Camera, Entity>("Camera")
		.addFunction("setDirection", &Camera::setDirection)
		.addFunction("getDirection", &Camera::getDirection)
		.addFunction("setUpVector", &Camera::setUpVector)
		.addFunction("getUpVector", &Camera::getUpVector)
		.addFunction("setFieldOfView", &Camera::setFieldOfView)
	.endClass();


	LuaBinding(luaState).beginExtendClass<Object, Entity>("Object")
		.addConstructor(LUA_SP(std::shared_ptr<Object>), LUA_ARGS(Object::constObjectGeometryPointer, Object::constShaderPointer))

		.addFunction("setObjectGeometry", &Object::setObjectGeometry)
		.addFunction("getObjectGeometry", &Object::getObjectGeometry)
		.addFunction("setShader", &Object::setShader)
		.addFunction("getShader", &Object::getShader)
	.endClass();


	LuaBinding(luaState).beginExtendClass<TexturedObject, Object>("TexturedObject")
		.addConstructor(LUA_SP(std::shared_ptr<TexturedObject>), LUA_ARGS(Object::constObjectGeometryPointer,
			Object::constShaderPointer, TexturedObject::constTexturePointer))

		.addFunction("setTexture", &TexturedObject::setTexture)
	.endClass();


	LuaBinding(luaState).beginExtendClass<ShadedObject, TexturedObject>("ShadedObject")
		.addConstructor(LUA_SP(std::shared_ptr<ShadedObject>), LUA_ARGS(Object::constObjectGeometryPointer,
			Object::constShaderPointer, ShadedObject::constTexturePointer))
	.endClass();


	LuaBinding(luaState).beginExtendClass<Light, Entity>("Light")
		.addConstructor(LUA_SP(std::shared_ptr<Light>), LUA_ARGS(glm::vec3, glm::vec3, glm::vec3, float))

		.addFunction("setDiffuseColor", &Light::setDiffuseColor)
		.addFunction("getDiffuseColor", &Light::getDiffuseColor)

		.addFunction("setSpecularColor", &Light::setSpecularColor)
		.addFunction("getSpecularColor", &Light::getSpecularColor)

		.addFunction("setPower", &Light::setPower)
		.addFunction("getPower", &Light::getPower)

		.addFunction("setOnState", &Light::setOnState)
		.addFunction("isOn", &Light::isOn)
	.endClass();


	LuaBinding(luaState).beginModule("Utils")
		.addFunction("logprint", [](const std::string& msg) // Use a lamda to make sure we don't need line numbers
		{
			Utils::directly_logprint(msg); // Use directly, we don't want C++ line numbers and files!
		})

		.addFunction("warn", [](const std::string& msg)
		{
			Utils::directly_warn(msg);
		})

		.addFunction("crash", [](const std::string& msg)
		{
			Utils::directly_crash(msg);
		})
	.endModule();


	// Entity must have at least one virtual function to be a base class
	//LuaBinding(luaState).beginExtendClass<Object, Entity>("Object")
	//.endClass();
}

// Running a script will probably not be too heavy since it will probably only be defining a bunch of callbacks.
// You should definitely call this after binding the interface.
void Script::run()
{
	try
	{
		mLuaContext.doString(mMainFileContents.c_str()); // Run the file!
	} catch(const std::exception& e)
	{
		std::string errorMessage = "Script '" + mName + "' failed!\nError: ";
		Utils::CRASH(errorMessage + e.what());
	}
}