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

#ifndef DEFINITIONS_HPP_
#define DEFINITIONS_HPP_

#include <LuaIntf.h>
#include <vector>
#include <map>
#include <memory>

// Default values
#define DEFAULT_GAME_NAME "SDL3D"
#define DEFAULT_GAME_WINDOW_WIDTH 800
#define DEFAULT_GAME_WINDOW_HEIGHT 600
#define DEFAULT_GAME_MAX_FRAMES_PER_SECOND 60

// Files and paths
#define LOG_FILE "Log.txt"
#define RESOURCE_PATH_PREFIX "resources/" // Added before all resources
#define SHADER_PATH_PREFIX "shaders/"
#define SCRIPT_PATH_PREFIX "scripts/"

#define MAIN_SCRIPT_NAME "main"
#define MAIN_SCRIPT_FILE MAIN_SCRIPT_NAME ".lua" // Concatenates both literals

// Texture types
#define BMP_TEXTURE 0
#define DDS_TEXTURE 1

// Scripting
namespace LuaIntf
{
	LUA_USING_SHARED_PTR_TYPE(std::shared_ptr);
	LUA_USING_LIST_TYPE(std::vector);
	LUA_USING_MAP_TYPE(std::map);
}

#endif /* DEFINITIONS_HPP_ */