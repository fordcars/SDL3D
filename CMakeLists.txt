#SDL3D

# The MIT License (MIT)

# Copyright (c) 2015 Carl Hewett

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# This file based off: https://github.com/mitsuhiko/immersedcode/blob/master/2011/4/6/sdl13-intro.rst

# TODO: Make it work with everything (Linux, compile with -ldl), make it funner to use.

# NOTES: CMAKE_CURRENT_SOURCE_DIR is the root directory of the project.
# Feel free to add include paths for static libraries if you want to.

cmake_minimum_required(VERSION 2.6)
project(SDL3D)

set(VS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
set(XCODE_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/SDL3D.app/Contents/MacOS/SDL3D)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resource)
set(RESOURCE_DIR_EXE resource) # The resource in the executable's directory
set(ARCHITECTURE x86)

set(GLAD_DIR ${LIB_DIR}/glad)
set(TINYOBJLOADER_DIR ${LIB_DIR}/tinyobjloader)
set(LUAINTF_DIR ${LIB_DIR}/LuaIntf/LuaIntf)

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
)

# Repairs conflicts with Windows libraries, or tries to. If it doesn't work, building as release will work.
SET(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:\"LIBCMT, MSVCRT, MSVPRTD\")
	
SET(CMAKE_CXX_FLAGS_DEBUG
	"${CMAKE_CXX_FLAGS_DEBUG} /wd4100 /wd4127 /wd4189 /wd4512 /wd4702"
	CACHE STRING "Debug builds CMAKE CXX flags " FORCE )

# The following code finds SDL in your checkout on OS X, Linux
# as well as Windows.
set(SDL_DIR ${LIB_DIR}/SDL
    CACHE STRING "Path to SDL" FORCE)
find_library(SDL_LIBRARY
    NAMES SDL2 SDL
    PATHS ${SDL_DIR}/lib
    PATH_SUFFIXES local/lib VisualC/SDL2/Release  VisualC/SDL/Release # Additional paths to check, appended after PATHS
    NO_DEFAULT_PATH
)
find_library(SDLMAIN_LIBRARY
    NAMES SDL2main SDLmain
    PATHS ${SDL_DIR}/lib
    PATH_SUFFIXES local/lib VisualC/SDL2main/Release VisualC/SDLmain/Release
    NO_DEFAULT_PATH
)

set(LUA_DIR ${LIB_DIR}/lua
    CACHE STRING "Path to Lua" FORCE)
find_library(LUA_LIBRARY
    NAMES lua lua5.3.2
    PATHS ${LUA_DIR}/lib
    PATH_SUFFIXES local/lib VisualC/lua/Release
    NO_DEFAULT_PATH
)

# We also need to find the system's OpenGL
find_package(OpenGL REQUIRED)

# On OS X we also have to add '-framework Cocoa' as library.  This is
# actually a bit of an hack but it's easy enough and reliable.
set(EXTRA_LIBRARIES "")
if (APPLE)
    set(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} "-framework Cocoa")
endif()

# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SDL_DIR}/include # For shorter includes
	${LUA_DIR}/include
	${GLAD_DIR}/include
	${TINYOBJLOADER_DIR}
	${LUAINTF_DIR}
	${LIB_DIR} # All other libraries
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

# Lastly we have to link the OpenGL libraries, SDL and the cocoa
# framework to our application.  The latter is only happening on
# OS X obviously.
target_link_libraries(
    SDL3D
    ${OPENGL_LIBRARIES}
    ${SDL_LIBRARY}
	${LUA_LIBRARY}
    ${SDLMAIN_LIBRARY}
    ${EXTRA_LIBRARIES}
)

# If this is Windows, copy the DLL next to the program
if(WIN32)
	# Copy SDL
    add_custom_command(TARGET SDL3D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL_DIR}/lib/SDL2.dll ${VS_OUTPUT_DIR}/SDL2.dll)
	
	# Copy Lua
    add_custom_command(TARGET SDL3D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LUA_DIR}/lib/lua.dll ${VS_OUTPUT_DIR}/lua.dll)
	
	# Copy resources
	add_custom_command(TARGET SDL3D POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${RESOURCE_DIR} ${VS_OUTPUT_DIR}/${RESOURCE_DIR_EXE})
endif()

# If this is Apple, copy the DLL into the Frameworks folder of the .app using the script frameworkify.py
if(APPLE)
	# SDL
    add_custom_command(TARGET SDL3D POST_BUILD
        COMMAND python scripts/frameworkify.py ${XCODE_OUTPUT_DIR}
        ${SDL_LIBRARY})
	
	# Lua
    add_custom_command(TARGET SDL3D POST_BUILD
        COMMAND python scripts/frameworkify.py ${XCODE_OUTPUT_DIR}
        ${LUA_LIBRARY})
	
	# Copy resources
	add_custom_command(TARGET SDL3D POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${RESOURCE_DIR} ${XCODE_OUTPUT_DIR}/${RESOURCE_DIR_EXE})
endif()