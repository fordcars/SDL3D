//// Copyright 2017 Carl Hewett
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

// UNITS:
// Frame related stuff: miliseconds
// Other times: seconds
// Coords: meters
// Angles: degrees

#include "Game.hpp"

#include "Definitions.hpp"
#include "Utils.hpp"
#include "SimpleTimer.hpp" // For game loop

#include "LuaRef.h" // For getting references from scripts
#include "SDL2/SDL_mixer.h"
#include "glm/glm.hpp"
#include "glad/glad.h" // For graphics and compability checks

#include <math.h>
#include <cstddef> // For std::size_t
#include <climits> // For CHAR_BIT
#include <array>
#include <cstdarg> // For glad debug callbacks

// With the help of:
// https://www.opengl.org/wiki/Tutorial1:_Creating_a_Cross_Platform_OpenGL_3.2_Context_in_SDL_%28C_/_SDL%29
// http://glew.sourceforge.net/basic.html

Game::Game()
{
	mName = DEFAULT_GAME_NAME; // Copy string
	mQuitting = false;

	mSize = glm::ivec2(DEFAULT_GAME_WINDOW_WIDTH, DEFAULT_GAME_WINDOW_HEIGHT);

	// Limits the frames per second
	// Time in miliseconds
	mMaxFramesPerSecond = DEFAULT_GAME_MAX_FRAMES_PER_SECOND; // Truncation
	mLastFrameTime = 0;

	// This is the length of a step, used for movement and everything, in ms. 8 ms makes 120 steps per second.
	// If the time of one frame is smaller than this value (ex: faster screens in the future), the game will slow down.
	mStepLength = 8;

	// These will be set later during initialization
	mMainWindow = nullptr;
	mMainContext = nullptr;

	init();
}

Game::~Game() // Deconstructor
{
	SDL_GL_DeleteContext(mMainContext);
	SDL_DestroyWindow(mMainWindow);
}

// Initializes all systems
// Returns false on failure
// You cannot print anything in the log before calling this method
bool Game::init()
{
	// Clear the data log first like that we can quickly print stuff
	Utils::clearDataOutput();
	Utils::LOGPRINT(std::string() + "Starting " + ENGINE_NAME + " v" + ENGINE_VERSION + "!");

	initSDL();
	initContext();

	// Libraries are initialized, now we can construct our managers!
	mInputManager = inputManagerPointer(new InputManager());
	mGraphicsManager = graphicsManagerPointer(new GraphicsManager(mSize));

	mResourceManager = resourceManagerPointer(new ResourceManager(mGraphicsManager, getBasePath()));
	mEntityManager = entityManagerPointer(new EntityManager(mGraphicsManager, glm::vec2(0.0f)));

	// Scripts
	// Only one script for now
	ResourceManager::scriptPointer mainScript = mResourceManager->addScript(MAIN_SCRIPT_NAME, MAIN_SCRIPT_FILE);

	mainScript->bindInterface(*this);
	// Run the script to get all of the definitions and all
	mainScript->run();
	// Run the script's init function
	mainScript->runFunction(MAIN_SCRIPT_FUNCTION_INIT);

	mEntityManager->getGameCamera().setAspectRatio(getAspectRatio());

	Utils::LOGPRINT("Game initialization completed!");
	return true;
}

// Returns false on failure
bool Game::initSDL()
{
	// SDL_INIT_AUDIO for SDL_mixer
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0)
	{
		// Failed
		Utils::CRASH_FROM_SDL("Unable to initialize SDL!");
		return false;
	}
	// Since we use this, we need to call gladLoadGLLoader() instead of gladLoadGL() later on
	if(SDL_GL_LoadLibrary(nullptr) < 0)
	{
		Utils::CRASH_FROM_SDL("Unable to load OpenGL from SDL!");
		return false;
	}
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) // 2 for stereo
	{
		Utils::CRASH_FROM_SDL("Unable to open SDL_mixer!");
		return false;
	}
	if(Mix_Init(MIX_INIT_FLAC | MIX_INIT_OGG) < 0)
	{
		Utils::CRASH_FROM_SDL("Unable to initialize SDL_mixer!");
		return false;
	}
	if(Mix_AllocateChannels(MAX_SOUND_CHANNELS) < 0)
	{
		Utils::CRASH_FROM_SDL("Unable to allocate SDL_mixer channels!");
		return false;
	}

	Utils::LOGPRINT("SDL initialization complete.");
	return true;
}

// Make sure OpenGL is initialized before calling this! (Hint: it is)
void postCallbackForGladDebug(const char* name, void* funcPtr, int argLength, ...)
{
	// No loop since a function should only generate one error
	GLenum error = glGetError();
	
	if(error != GL_NO_ERROR) // Something went wrong!
	{
		Utils::LOGPRINT("");
		Utils::WARN("OpenGL error during function '" + std::string(name) + "'!");
		Utils::LOGPRINT("OpenGL error code: '" + Utils::getGLErrorString(error) + "'.");
		Utils::LOGPRINT("");
	}
}

// This method initializes the graphics context, thus initializing OpenGL.
// You cannot call anything from OpenGL before calling this.
// Returns false on failure
bool Game::initContext()
{
	// Creating an OpenGL context also initializes OpenGL, so this has to be done here
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GRAPHICS_OPENGL_MAJOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GRAPHICS_OPENGL_MINOR_VERSION);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Create the window
	mMainWindow = SDL_CreateWindow(mName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mSize.x, mSize.y,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if(!mMainWindow) // If the window failed to create, crash
	{
		Utils::CRASH_FROM_SDL("Unable to create window!");
		return false;
	}

	// Create the context
	mMainContext = SDL_GL_CreateContext(mMainWindow); // Create OpenGL context!

	if(!mMainContext)
	{
		Utils::CRASH_FROM_SDL("Unable to create OpenGL context! This game requries OpenGL " +
			std::to_string(GRAPHICS_OPENGL_MAJOR_VERSION) + "." + std::to_string(GRAPHICS_OPENGL_MINOR_VERSION) +
			". Does your system support it? Try updating your graphics drivers!");
		return false;
	}

	// Load OpenGL functions
	if(!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		Utils::CRASH("GLAD failed to load OpenGL!");
		return false;
	}

	if(SDL_GL_MakeCurrent(mMainWindow, mMainContext) < 0)
	{
		Utils::CRASH_FROM_SDL("Unable to set current context!");
		return false;
	}

	SDL_GL_SetSwapInterval(1); // Kind of V-sync?

// Setup glad debug callback if glad was generated with the C/C++ Debug generator
// Note: right now it does not seem to work, but it looks like the problem
// is on glad's side, but I am not sure.
#ifdef GLAD_DEBUG
	glad_set_post_callback(&postCallbackForGladDebug);
#endif

	// Print OpenGL information
	// Since OpenGL gives GLubytes, we need to reinterpret them into chars (unsigned to signed)
	Utils::LOGPRINT("OpenGL information:");
	Utils::LOGPRINT(" - Vendor: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR))));
	Utils::LOGPRINT(" - Renderer: " + std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER))));
	Utils::LOGPRINT(" - Version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
	Utils::LOGPRINT("");

	if(!checkCompability()) // This function logs errors
	{
		Utils::CRASH("System not compatible, cancelling init.");
		return false;
	}

	checkForErrors();
	Utils::LOGPRINT("Context initialization complete.");

	return true;
}

// Checks if the game will work on the user's setup. Also checks for compiling environment compability.
bool Game::checkCompability()
{
	// Check compiler environment compability
	// http://stackoverflow.com/questions/13571898/c-opengl-glm-and-struct-padding

	// Check if glm::vecX are correct (just in-case)
	// This is vital since we are passing glm::vecX to GL as if it was a simple array. It must not have any padding!
	static_assert(sizeof(glm::vec4) == sizeof(float) * 4,
		"Compiling environment does not support glm::vec4 direct access. Please download another version of glm or contact the project's developpers.");
	static_assert(sizeof(glm::vec3) == sizeof(float) * 3,
		"Compiling environment does not support glm::vec3 direct access. Please download another version of glm or contact the project's developpers.");
	static_assert(sizeof(glm::vec2) == sizeof(float) * 2,
		"Compiling environment does not support glm::vec2 direct access. Please download another version of glm or contact the project's developpers.");

	// Check if types are the right sizes to make sure everything works with OpenGL
	static_assert(CHAR_BIT * sizeof(int) == 32,
		"Compiling environment does not have 32-bit ints. This could cause some compability problesm with OpenGL.");
	static_assert(CHAR_BIT * sizeof(float) == 32,
		"Compiling environment does not have 32-bit floats. This could cause some compability problesm with OpenGL.");

	// Check system's compability (runtime)
	// Check for GL extension compability
	std::array<std::string, 1> extensions = { "GL_EXT_texture_compression_s3tc" };

	GLint numExt;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);

	bool isCompatible = true; // True if the game is compatible with the system (after we are done checking the setup here)

	for(std::size_t i = 0; i < extensions.size(); i++)
	{
		bool foundExtension = false;

		for(int extI = 0; extI < numExt; extI++)
		{
			std::string extensionString = (reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, extI))); // const unsigned char * to std::string

			if(extensionString == extensions[i])
			{
				foundExtension = true;
				break; // Breaks from the most inner loop
			}
		}

		if(!foundExtension) // If the extension isn't found
		{
			Utils::WARN("The extension " + extensions[i] + " has not been found on your system.");
			isCompatible = false; // Not compatible!
								  // Don't break, let the user know what other extensions are not found (if it is the case)
		}
	}

	if(isCompatible)
	{
		Utils::LOGPRINT("Your system seems to be compatible with the game!");
		return true;
	}
	else
	{
		Utils::CRASH("Your system is not compatible with the game. Please take a look at the generated warnings.");
		return false;
	}
}

// Call each frame for safety. Do not call after deleting the OpenGL context.
// Returns true if errors were found
bool Game::checkForErrors()
{
	bool errorsFound = false;

	const int maxGLErrors = 1000; // Because meh, I don't like infinite loops
	bool finishedGLErrors = false;

	for(int i = 0; i<maxGLErrors; i++)
	{
		GLenum err = glGetError();

		if(err != GL_NO_ERROR) // There is an error
		{
			errorsFound = true;
			Utils::WARN("OpenGL error: " + Utils::getGLErrorString(err));
		}
		else // No (more) errors!
		{
			finishedGLErrors = true;
			break;
		}
	}

	if(!finishedGLErrors)
		// Is this even possible?
		Utils::WARN("Over " + std::to_string(maxGLErrors) + " OpenGL errors?!");

	// SDL
	// Most of the time the error will not be important since it includes internal diagnostics, so don't crash
	std::string message = SDL_GetError();

	if(!message.empty())
	{
		Utils::LOGPRINT("SDL message (most of the time you can ignore these): " + message);
		SDL_ClearError();
	}

	return errorsFound;
}

// Returns the directory where the game is being run (absolute path)
// In Mac application bundles, this returns the bundle.app/Contents/Resources directory.
// Fairly heavy function!
std::string Game::getBasePath()
{
	char *basePathPointer = SDL_GetBasePath(); // Must free this after!

											   // SDL failed to get the base path
	if(!basePathPointer)
	{
		SDL_free(basePathPointer); // Free the memory!

		std::string error = "Failed to get base path! Does your system support it? Search online on 'SDL_GetBasePath()' with your system for more information.";
		Utils::CRASH_FROM_SDL(error);
		return std::string();
	}

	std::string basePath = basePathPointer;
	SDL_free(basePathPointer); // SDL_free is a more cross-platform version of free()

	return basePath;
}

void Game::doMainLoop()
{
	SimpleTimer fpsTimer; // For calculating update delay and all
	int currentTime = fpsTimer.start();

	// Number of steps we need to do to be where we want to be
	int numberOfStepsToDo = (currentTime - mLastFrameTime) / mStepLength;

	doEvents();

	if(mLastFrameTime != 0) // Make sure everything is good before moving stuff!
	{
		for(int i = 0; i < numberOfStepsToDo; i++)
			step(static_cast<float>(numberOfStepsToDo));
	}

	mGraphicsManager->render(mMainWindow, mEntityManager);
	checkForErrors();

	mLastFrameTime = currentTime;

	int minTimePerFrame = 1000 / mMaxFramesPerSecond; // In miliseconds

													  // If the frame took les ticks than the minimum, delay the next frame, virtually always does this.
	if(fpsTimer.getTicks() < minTimePerFrame)
	{
		SDL_Delay(minTimePerFrame - fpsTimer.getTicks()); // Delay the remaining time for the ticks per frame wanted
	}
}

void Game::doEvents()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT) // For example, closing the window
			mQuitting = true;

		mInputManager->updateKeyByEvent(event);
	}
}

void Game::step(float divider) // Movement and all
{
	mEntityManager->step(divider);

	// Run the script's step()
	ResourceManager::scriptPointer mainScript = mResourceManager->findScript(MAIN_SCRIPT_NAME);
	mainScript->runFunction(MAIN_SCRIPT_FUNCTION_STEP);
}

// Safely stops all systems
void Game::cleanup()
{
	// Quit
	// From https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_10.html#SEC10
	for(int i = 0; i < 1000; i++) // I don't like infinite loops
	{
		if(Mix_Init(0))
			Mix_Quit();
		else
			break;
	}

	SDL_GL_DeleteContext(mMainContext);
	Mix_CloseAudio();
	SDL_Quit();

	Utils::LOGPRINT("Game quit successfully.");
	Utils::closeLogFile();
}

// Public Interface //

void Game::startMainLoop()
{
	while(!mQuitting) // While not quitting
		doMainLoop();

	cleanup();
	return; // Quit!
}

// Useful for Lua
void Game::quit()
{
	mQuitting = true;
}

float Game::getAspectRatio()
{
	return (static_cast<float>(mSize.x) / static_cast<float>(mSize.y));
}

void Game::setName(const std::string& name)
{
	mName = name;
	SDL_SetWindowTitle(mMainWindow, name.c_str());
}

std::string Game::getName()
{
	return mName;
}

// Sets the game's size (width and height)
void Game::setSize(glm::ivec2 size)
{
	mSize = size;
	SDL_SetWindowSize(mMainWindow, size.x, size.y);
	
	mGraphicsManager->setDrawSize(size);

	// Update camera
	mEntityManager->getGameCamera().setAspectRatio(getAspectRatio());
}

glm::vec2 Game::getSize()
{
	return mSize;
}

void Game::setMaxFramesPerSecond(int maxFPS)
{
	mMaxFramesPerSecond = maxFPS;
}

// Sets the game's main window position
// The coords are the top left corner
void Game::setMainWindowPosition(glm::ivec2 position)
{
	SDL_SetWindowPosition(mMainWindow, position.x, position.y);
}

glm::ivec2 Game::getMainWindowPosition()
{
	int x = 0;
	int y = 0;
	SDL_GetWindowPosition(mMainWindow, &x, &y);

	return glm::ivec2(x, y);
}

// Re-centers the game's main window on the first display
void Game::reCenterMainWindow()
{
	setMainWindowPosition(glm::vec2(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED));
}

Game::resourceManagerPointer Game::getResourceManager()
{
	return mResourceManager;
}

Game::inputManagerPointer Game::getInputManager()
{
	return mInputManager;
}

Game::entityManagerPointer Game::getEntityManager()
{
	return mEntityManager;
}

Game::graphicsManagerPointer Game::getGraphicsManager()
{
	return mGraphicsManager;
}
