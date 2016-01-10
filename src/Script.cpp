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

#include <SDL.h>
#include <exception> // For handling the script's exceptions

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
#include <EntityManager.hpp>
#include <Entity.hpp>
#include <Object.hpp>

#include <glm/glm.hpp>

// Binds all of the classes and functions. This creates our API!
// We need a Game instance to give it to the scripts.
void Script::bindInterface(Game& game)
{
	LuaState luaState = mLuaContext.state();

	LuaBinding(luaState).addFunction("getGame", [&game]
	{
		return game;
	});

	LuaBinding(luaState).beginClass<Game>("Game")
		.addFunction("getEntityManager", &Game::getEntityManager)
	.endClass();

	LuaBinding(luaState).beginClass<EntityManager>("EntityManager")
		.addFunction("getObjects", &EntityManager::getObjects)
	.endClass();

	LuaBinding(luaState).beginClass<Entity>("Entity")
		.addFunction("TEST", &Entity::TEST)
	.endClass();

	// Entity must have at least one virtual function to be a base class
	LuaBinding(luaState).beginExtendClass<Object, Entity>("Object")
	.endClass();
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