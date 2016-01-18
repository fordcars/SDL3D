# Copyright 2016 Carl Hewett
#
# This file is part of SDL3D.
#
# SDL3D is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SDL3D is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SDL3D. If not, see <http://www.gnu.org/licenses/>.

# This file based off: https://github.com/mitsuhiko/immersedcode/blob/master/2011/4/6/sdl13-intro.rst

# TODO: Make it work with everything (Linux, compile with -ldl), make it funner to use.

# NOTES: CMAKE_CURRENT_SOURCE_DIR is the root directory of the project.
# Feel free to add include paths for static libraries if you want to.

cmake_minimum_required(VERSION 2.6)
project(SDL3D)

set(VS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
set(XCODE_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/SDL3D.app/Contents/MacOS/SDL3D)

set(LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(RESOURCE_DIR_EXE resources) # The resource in the executable's directory
set(ARCHITECTURE x86)

set(GLAD_DIR ${LIBRARY_DIR}/glad)
set(TINYOBJLOADER_DIR ${LIBRARY_DIR}/tinyobjloader)
set(LUAINTF_DIR ${LIBRARY_DIR}/LuaIntf/LuaIntf)

# A list of header and source files
set(SOURCES
    src/SDL3D.cpp
	src/Game.cpp
	src/Utils.cpp
	src/ResourceManager.cpp
	src/InputManager.cpp
	src/SimpleTimer.cpp
	src/Camera.cpp
	src/ObjectGeometry.cpp
	src/ObjectGeometryGroup.cpp
	src/Shader.cpp
	src/Texture.cpp
	src/Object.cpp
	src/TexturedObject.cpp
	src/ShadedObject.cpp
	src/Light.cpp
	src/Entity.cpp
	src/EntityManager.cpp
	src/Script.cpp
	src/Sound.cpp
	
	# Static libs
	${GLAD_DIR}/src/glad.c
	${TINYOBJLOADER_DIR}/tiny_obj_loader.cc
)

 # Shows up in VisualStudio
set(HEADERS
    src/SDL3D.hpp
	src/Game.hpp
	src/Utils.hpp
	src/Definitions.hpp
	src/ResourceManager.hpp
	src/InputManager.hpp
	src/SimpleTimer.hpp
	src/Camera.hpp
	src/ObjectGeometry.hpp
	src/ObjectGeometryGroup.hpp
	src/Shader.hpp
	src/Texture.hpp
	src/GPUBuffer.hpp
	src/Object.hpp
	src/TexturedObject.hpp
	src/ShadedObject.hpp
	src/Light.hpp
	src/Entity.hpp
	src/EntityManager.hpp
	src/Script.hpp
	src/Sound.hpp
	src/IncludeLuaIntf.hpp
)

# Repairs conflicts with Windows libraries, or tries to. If it doesn't work, building as release will work.
SET(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:\"LIBCMT, MSVCRT, MSVPRTD\")
	
SET(CMAKE_CXX_FLAGS_DEBUG
	"${CMAKE_CXX_FLAGS_DEBUG} /wd4100 /wd4127 /wd4189 /wd4512 /wd4702"
	CACHE STRING "Debug builds CMAKE CXX flags " FORCE )

# The following code finds SDL2 in your checkout on OS X, Linux
# as well as Windows.
set(SDL2_DIR ${LIBRARY_DIR}/SDL2
    CACHE STRING "Path to SDL2" FORCE)
find_library(SDL2_LIBRARY # LIBRARY instead of LIB since OpenGL has LIBRARY as it's variable name
    NAMES SDL2
    PATHS ${SDL2_DIR}/lib
    PATH_SUFFIXES local/lib VisualC/SDL2/Release # Additional paths to check, appended after PATHS
    NO_DEFAULT_PATH
)
find_library(SDL2MAIN_LIBRARY
    NAMES SDL2main
    PATHS ${SDL2_DIR}/lib
    PATH_SUFFIXES local/lib VisualC/SDL2main/Release
    NO_DEFAULT_PATH
)
find_library(SDL2_MIXER_LIBRARY
    NAMES SDL2_mixer
    PATHS ${SDL2_DIR}/lib
    PATH_SUFFIXES local/lib VisualC/SDL2_mixer/Release
    NO_DEFAULT_PATH
)
find_library(NATIVE_MIDI_LIBRARY
    NAMES native_midi
    PATHS ${SDL2_DIR}/lib
    PATH_SUFFIXES local/lib VisualC/native_library/Release
    NO_DEFAULT_PATH
)
find_library(TIMIDITY_LIBRARY
    NAMES timidity
    PATHS ${SDL2_DIR}/lib
    PATH_SUFFIXES local/lib VisualC/timidity/Release
    NO_DEFAULT_PATH
)

# Lua
set(LUA_DIR ${LIBRARY_DIR}/lua
    CACHE STRING "Path to Lua" FORCE)
find_library(LUA_LIBRARY
    NAMES lua lua5.3.2
    PATHS ${LUA_DIR}/lib
    PATH_SUFFIXES local/lib VisualC/lua/Release
    NO_DEFAULT_PATH
)

# Box2D
set(BOX2D_DIR ${LIBRARY_DIR}/Box2D
    CACHE STRING "Path to Box2D" FORCE)
find_library(BOX2D_LIBRARY
    NAMES Box2D
    PATHS ${BOX2D_DIR}/lib
    PATH_SUFFIXES local/lib VisualC/Box2D/Release
    NO_DEFAULT_PATH
)

# We also need to find the system's OpenGL
find_package(OpenGL REQUIRED)

# On OS X we also have to add '-framework Cocoa' as library.  This is
# actually a bit of an hack but it's easy enough and reliable.
set(EXTRA_LIBRARIES "")
if(APPLE)
    set(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} "-framework Cocoa")
endif()

# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SDL2_DIR}/include # Includes SDL2 extensions, too
	${LUA_DIR}/include
	${BOX2D_DIR}/include
	${GLAD_DIR}/include
	${TINYOBJLOADER_DIR}
	${LUAINTF_DIR}
	${LIBRARY_DIR} # All other libraries
)

# Now we define what makes our executable.  First thing is the name,
# WIN32 is needed to make this a Win32 GUI application, MACOSX_BUNDLE
# activates bundle mode on OS X and the last two things are our source
# and header files this executable consists of.
add_executable(
    SDL3D
    WIN32
    MACOSX_BUNDLE
    ${SOURCES}
    ${HEADERS}
)

# Lastly we have to link the OpenGL libraries, SDL2 and the cocoa
# framework to our application.  The latter is only happening on
# OS X obviously.
target_link_libraries(
    SDL3D
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARY}
	${SDL2MAIN_LIBRARY}
	${SDL2_MIXER_LIBRARY}
	${NATIVE_MIDI_LIBRARY}
	${TIMIDITY_LIBRARY}
	${LUA_LIBRARY}
    ${EXTRA_LIBRARIES}
)

# If this is Windows, copy the DLL next to the program
if(WIN32)
	# Copy SDL
	# Copies the whole .dll directory
    add_custom_command(TARGET SDL3D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SDL2_DIR}/lib/x86 ${VS_OUTPUT_DIR})
	
	# Copy Lua
    add_custom_command(TARGET SDL3D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${LUA_DIR}/lib/x86 ${VS_OUTPUT_DIR})
	
	# Copy resources
	add_custom_command(TARGET SDL3D POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${RESOURCE_DIR} ${VS_OUTPUT_DIR}/${RESOURCE_DIR_EXE})
endif()

# If this is Apple, copy the DLL into the Frameworks folder of the .app using the script frameworkify.py
set(FRAMEWORKIFY_SCRIPT tools/Frameworkify/frameworkify.py)

if(APPLE)
	# SDL
    add_custom_command(TARGET SDL3D POST_BUILD
        COMMAND python ${FRAMEWORKIFY_SCRIPT} ${XCODE_OUTPUT_DIR}
        ${SDL2_LIBRARY})
	
	# Lua
    add_custom_command(TARGET SDL3D POST_BUILD
        COMMAND python ${FRAMEWORKIFY_SCRIPT} ${XCODE_OUTPUT_DIR}
        ${LUA_LIBRARY})
	
	# Copy resources
	# I think this works as long as the application is not a bundle, otherwise we have to change this. Info:
	# http://stackoverflow.com/questions/27181879/cmake-copy-icon-to-bundle-resources
	add_custom_command(TARGET SDL3D POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${RESOURCE_DIR} ${XCODE_OUTPUT_DIR}/${RESOURCE_DIR_EXE})
endif()