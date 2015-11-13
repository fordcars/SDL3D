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

# TODO: Make it work with everything (Linux, compile with -ldl), make it funner to use

# NOTES: No need to modify this to add static libraries
# CMAKE_CURRENT_SOURCE_DIR is the root directory of the project

cmake_minimum_required(VERSION 2.6)
project(SDL3D)

set(VS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
set(XCODE_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/SDL3D.app/Contents/MacOS/SDL3D)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resource)
set(RESOURCE_DIR_EXE resource) # The resource in the executable's directory
set(ARCHITECTURE x86)

set(GLAD_DIR ${LIB_DIR}/glad)

# A list of header and source files
set(SOURCES
    src/SDL3D.cpp
	src/Game.cpp
	src/Utils.cpp
	src/ResourceManager.cpp
	src/InputHandler.cpp
	src/SimpleTimer.cpp
	src/Camera.cpp
	src/ObjectTemplate.cpp
	src/TexturedObject.cpp
	src/Shader.cpp
	src/Texture.cpp
	src/BasicObject.cpp
	
	# Static libs
	${GLAD_DIR}/src/glad.c
)

 # Show up in VisualStudio
set(HEADERS
    src/SDL3D.hpp
	src/Game.hpp
	src/Utils.hpp
	src/Definitions.hpp
	src/ResourceManager.hpp
	src/InputHandler.hpp
	src/SimpleTimer.hpp
	src/Camera.hpp
	src/ObjectTemplate.hpp
	src/TexturedObject.hpp
	src/Shader.hpp
	src/Texture.hpp
	src/GLBuffer.hpp
	src/BasicObject.hpp
)

# Repairs conflicts with Windows libraries, or tries to. If it doesn't work, building as release will work.
SET(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:\"LIBCMT, MSVCRT, MSVPRTD\")
	
SET(CMAKE_CXX_FLAGS_DEBUG
	"${CMAKE_CXX_FLAGS_DEBUG} /wd4100 /wd4127 /wd4189 /wd4512 /wd4702"
	CACHE STRING "Debug builds CMAKE CXX flags " FORCE )

# The following code finds SDL2-2.0.3 in your checkout on OS X, Linux
# as well as Windows.
set(SDL_FOLDER ${LIB_DIR}/SDL2-2.0.3
    CACHE STRING "Path to SDL2-2.0.3" FORCE)
find_library(SDL_LIBRARY
    NAMES SDL2-2.0.3 SDL2
    PATHS ${SDL_FOLDER}/lib/${ARCHITECTURE}
    PATH_SUFFIXES local/lib VisualC/SDL2/Release
    NO_DEFAULT_PATH
)
find_library(SDLMAIN_LIBRARY
    NAMES SDL2main
    PATHS ${SDL_FOLDER}/lib/${ARCHITECTURE}
    PATH_SUFFIXES local/lib VisualC/SDL2main/Release
    NO_DEFAULT_PATH
)

# we also need to find the system's OpenGL version
find_package(OpenGL REQUIRED)

# on OS X we also have to add '-framework Cocoa' as library.  This is
# actually a bit of an hack but it's easy enough and reliable.
set(EXTRA_LIBS "")
if (APPLE)
    set(EXTRA_LIBS ${EXTRA_LIBS} "-framework Cocoa")
endif()

# our own include folder and the SDL one are additional folders we
# want to have on our path. These files here can now be included.
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SDL_FOLDER}/include # For shorter includes
	${GLAD_DIR}/include
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
    ${SDLMAIN_LIBRARY}
    ${EXTRA_LIBS}
)

# If this is Windows, copy the DLL next to the program
if(WIN32)
    add_custom_command(TARGET SDL3D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL_FOLDER}/lib/${ARCHITECTURE}/SDL2.dll ${VS_OUTPUT_DIR}/SDL2.dll)
		
	add_custom_command(TARGET SDL3D POST_BUILD # Copy resources
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${RESOURCE_DIR} ${VS_OUTPUT_DIR}/${RESOURCE_DIR_EXE})
endif()

# If this is Apple, copy the DLL into the Frameworks folder of the .app using the script frameworkify.py
if(APPLE)
    add_custom_command(TARGET SDL3D POST_BUILD
        COMMAND python scripts/frameworkify.py ${XCODE_OUTPUT_DIR}
        ${SDL_LIBRARY})
		
	add_custom_command(TARGET SDL3D POST_BUILD # Copy resources
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${RESOURCE_DIR} ${XCODE_OUTPUT_DIR}/${RESOURCE_DIR_EXE})
endif()