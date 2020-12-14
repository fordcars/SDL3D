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

// Don't overuse static function binding (use a normal function binding instead) to avoid
// confusing syntax ('.' instead of ':').

// This class is a single script. A script can be multiple files, but this is Lua's job.
// Happily, it looks like splitting the lua states per script, like here, is a good idea.
// http://lua-users.org/lists/lua-l/2003-11/msg00011.html

#include <Script.hpp>
#include <Utils.hpp>
#include <Game.hpp>

#include <exception> // For handling the script's exceptions
#include <memory> // For smart pointers
#include <vector>
#include <cstddef> // For std::size_t

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
	std::string clarifiedErrorMessage = "";

	// A function is returning an object that wasn't binded!
	if(errorMessage.find("bad argument #-2") != std::string::npos)
	{
		clarifiedError = true;
		clarifiedErrorMessage = "We suspect a function is returning an object of a non-binded class. If this is the case, this is a bug in this engine (!), please contact the developpers.";
	}

	// A module fonction is being called using the ':' syntax instead of the '.' syntax!
	if(errorMessage.find("on bad self") != std::string::npos)
	{
		clarifiedError = true;
		clarifiedErrorMessage = "We suspect you are attempting to call a function that is not binded to an object using ':'. Please use the '.' syntax instead!";
	}

	if(clarifiedError)
		Utils::WARN(clarifiedErrorMessage);

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
#include <PhysicsBody.hpp>

#include <Utils.hpp>

#include <Definitions.hpp>
#include <glm/glm.hpp>
#include <SDL_keycode.h> // For key codes

// Binds all of the classes and functions. This creates our API!
// We need a Game instance to give it to the scripts.
void Script::bindInterface(Game& game)
{
	// Notes:
	// If you get an 'arg #-2 is nil' runtime error, it is propably because the failing function is returning an object that is not binded!
	// You cannot give Lua different functions with the same name (overloaded Lua side). You have to give them different names (only on the Lua side).
	// Use a static_Cast to get the right overload.
	// You need to add a constructor to construct an object on the Lua side
	// You need to specify args with LUA_ARGS when binding a constructor with LUA_SP container (shared_ptr)
	// When you get a 'calling on bad self' runtime error from a module function call, make sure that you are calling this module fonction with the '.' sintax! (Not ':')
	// When you get a 'base class undefined' compile-time error, it might be because you are binding a static function in a class using
	// addFunction instead of addStaticFunction
	// When you get a 'expect userdata, got nil' runtime error, it means you were supposed to pass a class, but passed nil
	// Don't forget that Lua has 1-based tables/arrays!
	// When you get a 'use of undefined type' compile-time error, it might be because you are not specifying a getter/setter for a property

	LuaState luaState = mLuaContext.state();

	LuaBinding(luaState).addFunction("getGame", [&game]() -> Game& // The -> specifies the return type. The () are needed for this syntax.
	{
		return game;
	});

	LuaBinding(luaState).beginClass<Game>("Game")
		.addFunction("quit", &Game::quit) // &Game::quit returns quit()'s address
		.addFunction("setName", &Game::setName)
		.addFunction("getName", &Game::getName)
		.addFunction("setSize", &Game::setSize)
		.addFunction("getSize", &Game::getSize)

		.addFunction("setMaxFramesPerSecond", &Game::setMaxFramesPerSecond)
		.addFunction("setMainWindowPosition", &Game::setMainWindowPosition)
		.addFunction("getMainWindowPosition", &Game::getMainWindowPosition)
		.addFunction("reCenterMainWindow", &Game::reCenterMainWindow)

		.addFunction("setGraphicsBackgroundColor", &Game::setGraphicsBackgroundColor)
		.addFunction("getGraphicsBackgroundColor", &Game::getGraphicsBackgroundColor)

		.addFunction("getResourceManager", &Game::getResourceManager)
		.addFunction("getInputManager", &Game::getInputManager)
		.addFunction("getEntityManager", &Game::getEntityManager)
	.endClass();


	LuaBinding(luaState).beginModule("Engine")
		.addConstant("Name", ENGINE_NAME)
		.addConstant("Version", ENGINE_VERSION)
	.endModule();


	LuaBinding(luaState).beginClass<ResourceManager>("ResourceManager")
		.addFunction("addShader",
			// Specify which overload we want. Lua doesn't support functions with same names, though.
			// (ResourceManager::*) says this is a pointer to a function
			static_cast<ResourceManager::shaderPointer(ResourceManager::*) (const std::string&, const std::string&)>
				(&ResourceManager::addShader))

		.addFunction("addNamedShader",
			static_cast<ResourceManager::shaderPointer(ResourceManager::*) (const std::string&, const std::string&, const std::string&)>
				(&ResourceManager::addShader))

		.addFunction("findShader", &ResourceManager::findShader)
		.addFunction("clearShaders", &ResourceManager::clearShaders)

		.addFunction("addTexture",
			static_cast<ResourceManager::texturePointer(ResourceManager::*) (const std::string&, int)>
				(&ResourceManager::addTexture))

		.addFunction("addNamedTexture",
			static_cast<ResourceManager::texturePointer(ResourceManager::*) (const std::string&, const std::string&, int)>
				(&ResourceManager::addTexture))

		.addFunction("findTexture", &ResourceManager::findTexture)
		.addFunction("clearTextures", &ResourceManager::clearTextures)

		.addFunction("addObjectGeometryGroup",
			static_cast<ResourceManager::objectGeometryGroup_pointer(ResourceManager::*) (const std::string&)>
			(&ResourceManager::addObjectGeometryGroup))

		.addFunction("addNamedObjectGeometryGroup",
			static_cast<ResourceManager::objectGeometryGroup_pointer(ResourceManager::*) (const std::string&, const std::string&)>
			(&ResourceManager::addObjectGeometryGroup))

		.addFunction("addCustomObjectGeometryGroup",
			static_cast<ResourceManager::objectGeometryGroup_pointer(ResourceManager::*) (ResourceManager::objectGeometryGroup_pointer)>
			(&ResourceManager::addObjectGeometryGroup))

		.addFunction("findObjectGeometryGroup", &ResourceManager::findObjectGeometryGroup)
		.addFunction("clearObjectGeometryGroups", &ResourceManager::clearObjectGeometryGroups)

		.addFunction("addSound",
			static_cast<ResourceManager::soundPointer(ResourceManager::*) (const std::string&, int)>
			(&ResourceManager::addSound))

		.addFunction("addNamedSound",
			static_cast<ResourceManager::soundPointer(ResourceManager::*) (const std::string&, const std::string&, int)>
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
		// We can create a group to make custom objects from Lua
		.addConstructor(LUA_SP(std::shared_ptr<ObjectGeometryGroup>), LUA_ARGS(std::string))

		.addFunction("getName", &ObjectGeometryGroup::getName)
		.addFunction("addObjectGeometry", &ObjectGeometryGroup::addObjectGeometry)
		.addFunction("findObjectGeometry", &ObjectGeometryGroup::findObjectGeometry)
		.addFunction("getObjectGeometries", &ObjectGeometryGroup::getObjectGeometries)
	.endClass();


	LuaBinding(luaState).beginClass<ObjectGeometry>("ObjectGeometry")
		// You can create this type of object, but it will be stored as an std::shared_ptr
		// You need to specify LUA_ARGS with a LUA_SP container
		// There is a constructor like that we can construct or own geometry from Lua (without an object file).
		.addConstructor(LUA_SP(std::shared_ptr<ObjectGeometry>), LUA_ARGS(
			const std::string&,
			const ObjectGeometry::uintVector&,
			const ObjectGeometry::vec3Vector&,
			const ObjectGeometry::vec2Vector&,
			const ObjectGeometry::vec3Vector&))

		.addFunction("getName", &ObjectGeometry::getName)
		.addFunction("getIndexBuffer",
			// To get the non-const version
			static_cast<ObjectGeometry::uintBuffer&(ObjectGeometry::*) ()> (&ObjectGeometry::getIndexBuffer))

		.addFunction("getPositionBuffer",
			static_cast<ObjectGeometry::vec3Buffer&(ObjectGeometry::*) ()> (&ObjectGeometry::getPositionBuffer))

		.addFunction("getUVBuffer",
			static_cast<ObjectGeometry::vec2Buffer&(ObjectGeometry::*) ()> (&ObjectGeometry::getUVBuffer))

		.addFunction("getNormalBuffer",
			static_cast<ObjectGeometry::vec3Buffer&(ObjectGeometry::*) ()> (&ObjectGeometry::getNormalBuffer))
	.endClass();


	LuaBinding(luaState).beginClass<Sound>("Sound")
		.addFunction("getName", &Sound::getName)
		.addFunction("play", &Sound::play, LUA_ARGS(_def<int, 0>)) // Specify the loop argument is optional
		.addFunction("isPlaying", &Sound::isPlaying)
		.addFunction("halt", &Sound::halt)
		.addFunction("pause", &Sound::pause)
		.addFunction("isPaused", &Sound::isPaused)
		.addFunction("resume", &Sound::resume)

		.addFunction("fadeIn", &Sound::fadeIn, LUA_ARGS(float, _def<int, 0>)) // Specify the loop argument is optional
		.addFunction("fadeOut", &Sound::fadeOut)

		.addFunction("setVolume", &Sound::setVolume)
		.addFunction("getVolume", &Sound::getVolume)
	.endClass();


	LuaBinding(luaState).beginModule("SoundType")
		.addConstant("Music", SOUND_MUSIC)
		.addConstant("Chunk", SOUND_CHUNK)
	.endModule();


	// Bind a few useful GPUBuffers
	LuaBinding(luaState).beginClass<ObjectGeometry::uintBuffer>("GPUBuffer_uint")
		// We could of added a constructor, but I figured it would be pretty much useless, since we can't control
		// how buffers are sent to shaders from Lua.

		// Simplified 1-argument versions of setMutableData and setImmutableData
		.addFunction("setMutableData",
			static_cast<void (ObjectGeometry::uintBuffer::*)(const std::vector<unsigned int>&)>
			(&ObjectGeometry::uintBuffer::setMutableData))

		.addFunction("setImmutableData",
			static_cast<void (ObjectGeometry::uintBuffer::*)(const std::vector<unsigned int>&)>
			(&ObjectGeometry::uintBuffer::setImmutableData))

		// No argument version of read()
		.addFunction("read",
			static_cast<std::vector<unsigned int>(ObjectGeometry::uintBuffer::*)() const> (&ObjectGeometry::uintBuffer::read))
		
		.addFunction("modify", &ObjectGeometry::uintBuffer::modify)
	.endClass();


	LuaBinding(luaState).beginClass<ObjectGeometry::vec2Buffer>("GPUBuffer_vec2")
		// Simplified 1-argument versions of setMutableData and setImmutableData
		.addFunction("setMutableData",
			static_cast<void (ObjectGeometry::vec2Buffer::*)(const std::vector<glm::vec2>&)>
			(&ObjectGeometry::vec2Buffer::setMutableData))

		.addFunction("setImmutableData",
			static_cast<void (ObjectGeometry::vec2Buffer::*)(const std::vector<glm::vec2>&)>
			(&ObjectGeometry::vec2Buffer::setImmutableData))

		// No argument version of readData()
		.addFunction("readData",
			static_cast<std::vector<glm::vec2>(ObjectGeometry::vec2Buffer::*)() const> (&ObjectGeometry::vec2Buffer::read))

		.addFunction("modifyData", &ObjectGeometry::vec2Buffer::modify)
	.endClass();


	LuaBinding(luaState).beginClass<ObjectGeometry::vec3Buffer>("GPUBuffer_vec3")
		// Simplified 1-argument versions of setMutableData and setImmutableData
		.addFunction("setMutableData",
			static_cast<void (ObjectGeometry::vec3Buffer::*)(const std::vector<glm::vec3>&)>
			(&ObjectGeometry::vec3Buffer::setMutableData))

		.addFunction("setImmutableData",
			static_cast<void (ObjectGeometry::vec3Buffer::*)(const std::vector<glm::vec3>&)>
			(&ObjectGeometry::vec3Buffer::setImmutableData))

		// No argument version of readData()
		.addFunction("readData",
			static_cast<std::vector<glm::vec3>(ObjectGeometry::vec3Buffer::*)() const> (&ObjectGeometry::vec3Buffer::read))
		
		.addFunction("modifyData", &ObjectGeometry::vec3Buffer::modify)
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
		.addConstant("TAB", SDLK_TAB)
		.addConstant("RETURN", SDLK_RETURN)
		.addConstant("ESCAPE", SDLK_ESCAPE)
		.addConstant("EXCLAIM", SDLK_EXCLAIM)
		.addConstant("QUOTEDBL", SDLK_QUOTEDBL)
		.addConstant("HASH", SDLK_HASH)
		.addConstant("DOLLAR", SDLK_DOLLAR)
		.addConstant("AMPERSAND", SDLK_AMPERSAND)
		.addConstant("QUOTE", SDLK_QUOTE)
		.addConstant("LEFTPAREN", SDLK_LEFTPAREN)
		.addConstant("RIGHTPAREN", SDLK_RIGHTPAREN)
		.addConstant("ASTERISK", SDLK_ASTERISK)
		.addConstant("PLUS", SDLK_PLUS)
		.addConstant("COMMA", SDLK_COMMA)
		.addConstant("MINUS", SDLK_MINUS)
		.addConstant("PERIOD", SDLK_PERIOD)
		.addConstant("FSLASH", SDLK_SLASH)
		.addConstant("BSLASH", SDLK_BACKSLASH)
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
		.addConstant("N0", SDLK_0)
		.addConstant("N1", SDLK_1)
		.addConstant("N2", SDLK_2)
		.addConstant("N3", SDLK_3)
		.addConstant("N4", SDLK_4)
		.addConstant("N5", SDLK_5)
		.addConstant("N6", SDLK_6)
		.addConstant("N7", SDLK_7)
		.addConstant("N8", SDLK_8)
		.addConstant("N9", SDLK_9)
		.addConstant("KP0", SDLK_KP_0)
		.addConstant("KP1", SDLK_KP_1)
		.addConstant("KP2", SDLK_KP_2)
		.addConstant("KP3", SDLK_KP_3)
		.addConstant("KP4", SDLK_KP_4)
		.addConstant("KP5", SDLK_KP_5)
		.addConstant("KP6", SDLK_KP_6)
		.addConstant("KP7", SDLK_KP_7)
		.addConstant("KP8", SDLK_KP_8)
		.addConstant("KP9", SDLK_KP_9)
		.addConstant("F1", SDLK_F1)
		.addConstant("F2", SDLK_F2)
		.addConstant("F3", SDLK_F3)
		.addConstant("F4", SDLK_F4)
		.addConstant("F5", SDLK_F5)
		.addConstant("F6", SDLK_F6)
		.addConstant("F7", SDLK_F7)
		.addConstant("F8", SDLK_F8)
		.addConstant("F9", SDLK_F9)
		.addConstant("F10", SDLK_F10)
		.addConstant("F11", SDLK_F11)
		.addConstant("F12", SDLK_F12)
	.endModule();


	LuaBinding(luaState).beginClass<EntityManager>("EntityManager")
		.addFunction("getGameCamera", &EntityManager::getGameCamera)
		.addFunction("addObject", &EntityManager::addObject)

		.addFunction("removeObjectByIndex",
			static_cast<EntityManager::objectPointer(EntityManager::*) (std::size_t)>
			(&EntityManager::removeObject))

		.addFunction("removeObject",
			static_cast<bool(EntityManager::*) (EntityManager::objectPointer)>
			(&EntityManager::removeObject))

		.addFunction("getObjects", &EntityManager::getObjects)

		.addFunction("addLight", &EntityManager::addLight)

		.addFunction("removeLightByIndex",
			static_cast<EntityManager::lightPointer(EntityManager::*) (std::size_t)>
			(&EntityManager::removeLight))

		.addFunction("removeLight",
			static_cast<bool(EntityManager::*) (EntityManager::lightPointer)>
			(&EntityManager::removeLight))

		.addFunction("getLights", &EntityManager::getLights)

		.addFunction("setPhysicsTimePerStep", &EntityManager::setPhysicsTimePerStep)
		.addFunction("getPhysicsTimePerStep", &EntityManager::getPhysicsTimePerStep)
	.endClass();


	LuaBinding(luaState).beginClass<Entity>("Entity")
		.addFunction("getPhysicsBody",
			static_cast<PhysicsBody& (Entity::*)()> (&Entity::getPhysicsBody))
	.endClass();


	LuaBinding(luaState).beginClass<PhysicsBody>("PhysicsBody")
		.addFunction("calculateShapes",
			static_cast<bool (PhysicsBody::*)()> (&PhysicsBody::calculateShapes))
		.addFunction("calculateShapesUsingObjectGeometry",
			static_cast<bool (PhysicsBody::*)(bool, glm::vec3)> (&PhysicsBody::calculateShapes))
		.addFunction("calculateShapesFromRadius",
			static_cast<bool (PhysicsBody::*)(float)> (&PhysicsBody::calculateShapes))

		.addFunction("setDensity", &PhysicsBody::setDensity)
		.addFunction("getDensity", &PhysicsBody::getDensity)

		.addFunction("setFriction", &PhysicsBody::setFriction)
		.addFunction("getFriction", &PhysicsBody::getFriction)

		.addFunction("setRestitution", &PhysicsBody::setRestitution)
		.addFunction("getRestitution", &PhysicsBody::getRestitution)

		.addFunction("setWorldFriction", &PhysicsBody::setWorldFriction)
		.addFunction("getWorldFriction", &PhysicsBody::getWorldFriction)

		.addFunction("isCircular", &PhysicsBody::isCircular)
		.addFunction("getRadius", &PhysicsBody::getRadius)
		.addFunction("getType", &PhysicsBody::getType)

		.addFunction("setPosition", &PhysicsBody::setPosition)
		.addFunction("getPosition", &PhysicsBody::getPosition)

		.addFunction("setRotation", &PhysicsBody::setRotation)
		.addFunction("setRotationInRadians", &PhysicsBody::setRotationInRadians)
		.addFunction("getRotation", &PhysicsBody::getRotation)
		.addFunction("getRotationInRadians", &PhysicsBody::getRotationInRadians)

		.addFunction("setVelocity", &PhysicsBody::setVelocity)
		.addFunction("getVelocity", &PhysicsBody::getVelocity)

		.addFunction("setBullet", &PhysicsBody::setBullet)
		.addFunction("isBullet", &PhysicsBody::isBullet)
		.addFunction("setFixtedRotation", &PhysicsBody::setFixtedRotation)
		.addFunction("isFixtedRotation", &PhysicsBody::isFixtedRotation)

		.addFunction("getShapesLocal2DCenter", &PhysicsBody::getShapesLocal2DCenter)
		.addFunction("getShapesLocal3DCenter", &PhysicsBody::getShapesLocal3DCenter)

		.addFunction("renderDebugShapeWithCoord",
			static_cast<void (PhysicsBody::*)(Object::constShaderPointer, const Camera*, float)> (&PhysicsBody::renderDebugShape))
		.addFunction("renderDebugShape",
			static_cast<void (PhysicsBody::*)(Object::constShaderPointer, const Camera*)> (&PhysicsBody::renderDebugShape))
	.endClass();


	LuaBinding(luaState).beginModule("PhysicsBodyType")
		.addConstant("Ignored", PHYSICS_BODY_IGNORED)
		.addConstant("Static", PHYSICS_BODY_STATIC)
		.addConstant("Kinematic", PHYSICS_BODY_KINEMATIC)
		.addConstant("Dynamic", PHYSICS_BODY_DYNAMIC)
	.endModule();


	LuaBinding(luaState).beginExtendClass<Camera, Entity>("Camera")
		.addFunction("setDirection", &Camera::setDirection)
		.addFunction("getDirection", &Camera::getDirection)
		.addFunction("setUpVector", &Camera::setUpVector)
		.addFunction("getUpVector", &Camera::getUpVector)
		.addFunction("setFieldOfView", &Camera::setFieldOfView)

		.addFunction("setNearClippingDistance", &Camera::setNearClippingDistance)
		.addFunction("getNearClippingDistance", &Camera::getNearClippingDistance)
		.addFunction("setFarClippingDistance", &Camera::setFarClippingDistance)
		.addFunction("getFarClippingDistance", &Camera::getFarClippingDistance)
	.endClass();


	LuaBinding(luaState).beginExtendClass<Object, Entity>("Object")
		.addConstructor(LUA_SP(std::shared_ptr<Object>), LUA_ARGS(Object::constObjectGeometryPointer, Object::constShaderPointer,
			bool, int))

		.addFunction("setObjectGeometry", &Object::setObjectGeometry)
		.addFunction("getObjectGeometry", &Object::getObjectGeometry)
		.addFunction("setShader", &Object::setShader)
		.addFunction("getShader", &Object::getShader)
	.endClass();


	LuaBinding(luaState).beginExtendClass<TexturedObject, Object>("TexturedObject")
		.addConstructor(LUA_SP(std::shared_ptr<TexturedObject>), LUA_ARGS(Object::constObjectGeometryPointer, Object::constShaderPointer,
			TexturedObject::constTexturePointer, bool, int))
		.addFunction("setTexture", &TexturedObject::setTexture)
	.endClass();


	LuaBinding(luaState).beginExtendClass<ShadedObject, TexturedObject>("ShadedObject")
		.addConstructor(LUA_SP(std::shared_ptr<ShadedObject>), LUA_ARGS(Object::constObjectGeometryPointer, Object::constShaderPointer,
			TexturedObject::constTexturePointer, bool, int))
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
		.addFunction("logprint", [](const std::string& msg) // Use a lamda to not need to specify line numbers (those are defaulted arguments in the definition)
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
	
	// Basic glm bindings
	LuaBinding(luaState).beginClass<glm::ivec2>("IVec2") // Small glm::ivec2 binding
		.addConstructor(LUA_ARGS(int, int))

		.addVariable("x", &glm::ivec2::x) // Accessible in Lua using the '.' syntax
		.addVariable("y", &glm::ivec2::y)

		.addVariable("r", &glm::ivec2::r)
		.addVariable("g", &glm::ivec2::g)
	.endClass();

	LuaBinding(luaState).beginClass<glm::vec2>("Vec2")
		.addConstructor(LUA_ARGS(float, float))

		.addVariable("x", &glm::vec2::x) // Accessible in Lua using the '.' syntax
		.addVariable("y", &glm::vec2::y)

		.addVariable("r", &glm::vec2::r)
		.addVariable("g", &glm::vec2::g)
		
		// Can't have multiple constructors in Lua (no function overloading)
		.addStaticFunction("fromVec3", [](const glm::vec3& vector) // This name works well with Lua
		{
			return glm::vec2(vector);
		})

		.addStaticFunction("fromVec4", [](const glm::vec4& vector)
		{
			return glm::vec2(vector);
		})

		// Couldn't figure out how to use glm's built in basic arithmetics with Lua (if possible)
		// Arithmetics
		.addStaticFunction("add", [](const glm::vec2& left, const glm::vec2& right)
		{
			return (left + right);
		})

		.addStaticFunction("sub", [](const glm::vec2& left, const glm::vec2& right)
		{
			return (left - right);
		})

		.addStaticFunction("scalarMul", [](const glm::vec2& vector, float scalar)
		{
			return (vector * scalar);
		})

		.addStaticFunction("scalarDiv", [](const glm::vec2& vector, float scalar)
		{
			return (vector / scalar);
		})

		.addStaticFunction("length", [](const glm::vec2& vector)
		{
			return glm::length(vector);
		})

		.addStaticFunction("normalize", [](const glm::vec2& vector)
		{
			return glm::normalize(vector);
		})

		.addStaticFunction("dot", [](const glm::vec2& vector1, const glm::vec2& vector2)
		{
			return glm::dot(vector1, vector2);
		})
	.endClass();


	LuaBinding(luaState).beginClass<glm::vec3>("Vec3")
		.addConstructor(LUA_ARGS(float, float, float))

		.addVariable("x", &glm::vec3::x)
		.addVariable("y", &glm::vec3::y)
		.addVariable("z", &glm::vec3::z)

		.addVariable("r", &glm::vec3::r)
		.addVariable("g", &glm::vec3::g)
		.addVariable("b", &glm::vec3::b)

		.addStaticFunction("fromVec4", [](const glm::vec4& vector)
		{
			return glm::vec3(vector);
		})

		.addStaticFunction("add", [](const glm::vec3& left, const glm::vec3& right)
		{
			return (left + right);
		})

		.addStaticFunction("sub", [](const glm::vec3& left, const glm::vec3& right)
		{
			return (left - right);
		})

		.addStaticFunction("scalarMul", [](const glm::vec3& vector, float scalar)
		{
			return (vector * scalar);
		})

		.addStaticFunction("scalarDiv", [](const glm::vec3& vector, float scalar)
		{
			return (vector / scalar);
		})

		.addStaticFunction("length", [](const glm::vec3& vector)
		{
			return glm::length(vector);
		})

		.addStaticFunction("normalize", [](const glm::vec3& vector)
		{
			return glm::normalize(vector);
		})

		.addStaticFunction("dot", [](const glm::vec3& vector1, const glm::vec3& vector2)
		{
			return glm::dot(vector1, vector2);
		})
	.endClass();


	LuaBinding(luaState).beginClass<glm::vec4>("Vec4")
		.addConstructor(LUA_ARGS(float, float, float, float))

		.addVariable("x", &glm::vec4::x)
		.addVariable("y", &glm::vec4::y)
		.addVariable("z", &glm::vec4::z)
		.addVariable("w", &glm::vec4::w)

		.addVariable("r", &glm::vec4::r)
		.addVariable("g", &glm::vec4::g)
		.addVariable("b", &glm::vec4::b)
		.addVariable("a", &glm::vec4::a)

		.addStaticFunction("add", [](const glm::vec4& left, const glm::vec4& right)
		{
			return (left + right);
		})

		.addStaticFunction("sub", [](const glm::vec4& left, const glm::vec4& right)
		{
			return (left - right);
		})

		.addStaticFunction("scalarMul", [](const glm::vec4& vector, float scalar)
		{
			return (vector * scalar);
		})

		.addStaticFunction("scalarDiv", [](const glm::vec4& vector, float scalar)
		{
			return (vector / scalar);
		})

		.addStaticFunction("length", [](const glm::vec4& vector)
		{
			return glm::length(vector);
		})

		.addStaticFunction("normalize", [](const glm::vec4& vector)
		{
			return glm::normalize(vector);
		})

		.addStaticFunction("dot", [](const glm::vec4& vector1, const glm::vec4& vector2)
		{
			return glm::dot(vector1, vector2);
		})
	.endClass();
}

// Running a script will probably not be too heavy since it will probably only be defining a bunch of callbacks.
// You should definitely call this after binding the interface.
bool Script::run()
{
	try
	{
		mLuaContext.doString(mMainFileContents.c_str()); // Run the file!
	} catch(const std::exception& e)
	{
		std::string scriptErrorMessage = e.what();

		clarifyError(scriptErrorMessage);
		std::string errorMessage = "Script '" + mName + "' failed!\nError: " + scriptErrorMessage;
		Utils::CRASH(errorMessage);

		return false;
	}

	return true;
}

// Run a string of script code directly on this script
bool Script::runString(const std::string& scriptCode)
{
	try
	{
		mLuaContext.doString(scriptCode.c_str()); // Run the file!
	}
	catch(const std::exception& e)
	{
		std::string scriptErrorMessage = e.what();

		clarifyError(scriptErrorMessage);
		std::string errorMessage = "Running string on script '" + mName + "' failed!\nError: " + scriptErrorMessage;
		Utils::CRASH(errorMessage);

		return false;
	}

	return true;
}

// Gets a reference from the script (for example, a variable or a function)
LuaRef Script::getReference(const std::string& referenceName)
{
	LuaState luaState = mLuaContext.state();
	return LuaRef(luaState, referenceName.c_str());
}

// Checks for errors, too
void Script::runFunction(const std::string& functionName)
{
	try
	{
		getReference(functionName)();
	}
	catch(const std::exception& e)
	{
		std::string scriptErrorMessage = e.what();

		clarifyError(scriptErrorMessage);
		std::string errorMessage = "Running function in script '" + mName + "' from C++ failed!\nError: " + scriptErrorMessage;
		Utils::CRASH(errorMessage);
	}
}