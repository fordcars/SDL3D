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

#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <vector>
#include <map>
#include <memory>

#include <glad/glad.h>

#define ENGINE_NAME "SDL3D"
#define ENGINE_VERSION "0.1"

// Graphics
#define GRAPHICS_OPENGL_MAJOR_VERSION 3
#define GRAPHICS_OPENGL_MINOR_VERSION 3

#define GRAPHICS_RASTERIZE_FACE GL_FRONT_AND_BACK
#define GRAPHICS_RASTERIZE_MODE GL_FILL

#define GRAPHICS_MAX_LIGHTS 256

// Defines how many chunk sounds can exist. A super high number exceeding memory could segfault!
#define MAX_SOUND_CHANNELS 50

// Default values
#define DEFAULT_GAME_NAME "SDL3D Game"
#define DEFAULT_GAME_WINDOW_WIDTH 800
#define DEFAULT_GAME_WINDOW_HEIGHT 600
#define DEFAULT_GAME_MAX_FRAMES_PER_SECOND 60

// Files and paths
#define LOG_FILE "Log.txt"
#define RESOURCE_PATH_PREFIX "resources/" // Added before all resources
#define SHADER_PATH_PREFIX "shaders/"
#define SCRIPT_PATH_PREFIX "scripts/"

// Scripts
#define MAIN_SCRIPT_NAME "main"
#define MAIN_SCRIPT_FILE MAIN_SCRIPT_NAME ".lua" // Concatenates both literals

#define MAIN_SCRIPT_FUNCTION_INIT "gameInit"
#define MAIN_SCRIPT_FUNCTION_STEP "gameStep"

// Texture types
#define TEXTURE_BMP 0
#define TEXTURE_DDS 1

// Sound types
#define SOUND_MUSIC 0
#define SOUND_CHUNK 1 // Short sound effects would use this type

// Physics
#define PHYSICS_PIXELS_PER_METER 10.0f // Float

#define PHYSICS_DEFAULT_WORLD_FRICTION 2.0f

#define PHYSICS_BODY_IGNORED 0    // Ignored by the physics engine, still moves 'cus we are nice
#define PHYSICS_BODY_STATIC 1     // Does not move, collides
#define PHYSICS_BODY_KINEMATIC 2  // Moves and collides, but does not move according to those collisions
#define PHYSICS_BODY_DYNAMIC 3    // Moves and collides like in real life

// The closest vertices can be. Box2D has this value at 0.05f, and poly2tri at anything but 0.
// Vertices too close will be ignore by the physics engine.
#define PHYSICS_BODY_MIN_VERTEX_DISTANCE 0.06f

// Constants
#define CONST_PI 3.14159f
#define CONST_TWO_PI 2 * CONST_PI

#endif // DEFINITIONS_HPP
