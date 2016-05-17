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

// LuaIntf::LuaContext does not support copying or moving yet it seems

#ifndef SCRIPT_MANAGER_HPP
#define SCRIPT_MANAGER_HPP

#include <IncludeLuaIntf.hpp>
#include <LuaRef.h>
#include <string>

// Forward declare Game since we don't want to include Game.hpp again from ResourceManager.hpp!
// This avoids circular dependencies.
class Game;
class Script
{
private:
	std::string mName;
	std::string mMainFilePath;
	std::string mMainFileContents; // The contents of the main script file

	// You cannot store a LuaIntf::LuaState for a long time! Those can only be temporary.
	// You can only store a LuaIntf::LuaContext. You can get a state from the context.
	// I learnt this the hard way.
	LuaIntf::LuaContext mLuaContext;

	bool clarifyError(const std::string& errorMessage);

public:
	Script(const std::string& name, const std::string& mainFilePath, const std::string& absoluteRequirePath);
	~Script();

	bool setLuaRequirePath(const std::string& absolutePath);

	void bindInterface(Game& game);
	bool run();
	bool runString(const std::string& scriptCode);
	LuaIntf::LuaRef getReference(const std::string& referenceName);
	void runFunction(const std::string& functionName);
};

#endif // SCRIPT_MANAGER_HPP