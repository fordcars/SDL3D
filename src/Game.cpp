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

// UNITS:
// Frame related stuff: miliseconds
// Other times: seconds
// Coords: meters
// Angles: degrees

#include <Game.hpp> // Angled brakets ("<>") involve a more general search

#include <Definitions.hpp> 
#include <Utils.hpp>
#include <SimpleTimer.hpp> // For game loop

#include <LuaRef.h> // For getting references from scripts
#include <SDL_mixer.h>

#include <string> // No .h for c++

#include <math.h>
#include <memory> // For smart pointers

#include <glm/glm.hpp>
#include <glad/glad.h> // For compability checks

// With the help of:
// https://www.opengl.org/wiki/Tutorial1:_Creating_a_Cross_Platform_OpenGL_3.2_Context_in_SDL_%28C_/_SDL%29
// http://glew.sourceforge.net/basic.html

Game::Game()
	: mEntityManager(glm::vec2(0.0f),  1 / static_cast<float>(DEFAULT_GAME_MAX_FRAMES_PER_SECOND))
{
	mName = DEFAULT_GAME_NAME; // Copy string

	mSize.x = DEFAULT_GAME_WINDOW_WIDTH;
	mSize.y = DEFAULT_GAME_WINDOW_HEIGHT;

	// Limits the frames per second
	// Time in miliseconds
	mMaxFramesPerSecond = DEFAULT_GAME_MAX_FRAMES_PER_SECOND; // Truncation
	mLastFrameTime = 0;

	// This is the length of a step, used for movement and everything, in ms. 8 ms makes 120 steps per second.
	// If the time of one frame is smaller than this value (ex: faster screens in the future), the game will slow down.
	mStepLength = 8;

	mGraphicsBackgroundColor = glm::vec3(0.0f, 0.0f, 1.0f);

	mInitialized = false;
	mQuitting = false;

	// These will be set later
	mMainWindow = nullptr;
	mMainContext = nullptr;
}

Game::~Game() // Deconstructor
{
	quit();
}

// Static
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

// Checks if the game will work on the user's setup. Also checks for compiling environment compability.
bool Game::checkCompability()
{
	// Check compiler environment compability
	// http://stackoverflow.com/questions/13571898/c-opengl-glm-and-struct-padding

	// Check if glm::vecX are correct (just in-case)
	// This is vital since we are passing glm::vecX to GL as if it was a simple array. It must not have any padding!
	static_assert(sizeof(glm::vec4) == sizeof(GLfloat) * 4,
		"Compiling environment does not support glm::vec4 direct access. Please download another version of glm or contact the project's developpers.");
	static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3,
		"Compiling environment does not support glm::vec3 direct access. Please download another version of glm or contact the project's developpers.");
	static_assert(sizeof(glm::vec2) == sizeof(GLfloat) * 2,
		"Compiling environment does not support glm::vec2 direct access. Please download another version of glm or contact the project's developpers.");

	// Check system's compability (runtime)
	// Check for GL extension compability
	std::string extensions[] = {"GL_EXT_texture_compression_s3tc"};
	int numberOfExtensions = 1;

	GLint numExt;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);

	bool isCompatible = true; // True if the game is compatible with the system (after we are done checking the setup here)
	
	for(int i = 0; i < numberOfExtensions; i++)
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

void Game::setupGraphics() // VAO and OpenGL options
{
	// Make sure the OpenGL context extends over the whole screen
	glViewport(0, 0, mSize.x, mSize.y);

	GLuint vertexArrayID; // VAO - vertex array object
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
}

void Game::initMainLoop() // Initialize a few things before the main loop
{
	mResourceManager.setBasePath(getBasePath());

	// Scripts
	// Only one script for now
	ResourceManager::scriptPointer mainScript = mResourceManager.addScript(MAIN_SCRIPT_NAME, MAIN_SCRIPT_FILE);

	mainScript->bindInterface(*this);
	// Run the script to get all of the definitions and all
	mainScript->run();
	// Run the script's init function
	mainScript->runFunction(MAIN_SCRIPT_FUNCTION_INIT);

	mEntityManager.getGameCamera().setAspectRatio(calculateAspectRatio());

	EntityManager::lightPointer light(new Light(glm::vec3(4, 4, 4), glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), 60));
	mEntityManager.addLight(light);
}

void Game::cleanUp() // Cleans up everything. Call before quitting
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

	Mix_CloseAudio();

	SDL_GL_DeleteContext(mMainContext);
	SDL_DestroyWindow(mMainWindow);
	SDL_Quit();

	Utils::LOGPRINT("Game quit successfully.");
	Utils::closeLogFile();
}

void Game::doEvents()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		mInputManager.updateKeyByEvent(event);
		mInputManager.updateMouseMovement(event);

		if(event.type == SDL_QUIT)
			quit();
	}
}

void Game::checkForErrors() // Call each frame for safety. Do not call after deleting the OpenGL context.
{
	const int maxGLErrors = 1000;
	bool finishedGLErrors = false;

	for(int i=0; i<maxGLErrors; i++) // Because meh, I don't like infinite loops
	{
		GLenum err = glGetError();
		
		if(err!=GL_NO_ERROR) // There is an error
		{
			if(err == GL_INVALID_ENUM)
				Utils::WARN("OpenGL error: GL_INVALID_ENUM");
			else if(err == GL_INVALID_VALUE)
				Utils::WARN("OpenGL error: GL_INVALID_VALUE");
			else if(err == GL_INVALID_OPERATION)
				Utils::WARN("OpenGL error: GL_INVALID_OPERATION");
			else if(err == GL_STACK_OVERFLOW)
				Utils::WARN("OpenGL error: GL_STACK_OVERFLOW");
			else if(err == GL_STACK_UNDERFLOW)
				Utils::WARN("OpenGL error: GL_STACK_UNDERFLOW");
			else if(err == GL_OUT_OF_MEMORY)
				Utils::WARN("OpenGL error: GL_OUT_OF_MEMORY");
		} else // No (more) errors!
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
}

float Game::calculateAspectRatio()
{
	return (  static_cast<float>(mSize.x) / static_cast<float>(mSize.y)  );
}

void Game::step(float divider) // Movement and all
{
	mEntityManager.step(divider);

	// Run the script's step()
	ResourceManager::scriptPointer mainScript = mResourceManager.findScript(MAIN_SCRIPT_NAME);
	mainScript->runFunction(MAIN_SCRIPT_FUNCTION_STEP);
}

void Game::resetGraphics()
{
	// Set clear color
	glClearColor(mGraphicsBackgroundColor.r, mGraphicsBackgroundColor.g, mGraphicsBackgroundColor.b,
		1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear both color buffers and depth (z-indexes) buffers to push a clean buffer when done

	// It looks like it's better to call these each frame
	glEnable(GL_DEPTH_TEST);// Enable depth test (check if z is closer to the screen than last fragement's z)
	glDepthFunc(GL_LESS); // Accept the fragment closer to the camera

	// Cull triangles which normal is not towards the camera
	// If there are holes in the model because of this, click the "invert normals" button in your 3D modeler.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GRAPHICS_RASTERIZE_FACE, GRAPHICS_RASTERIZE_MODE);
}

void Game::render()
{
	mEntityManager.render();
	SDL_GL_SwapWindow(mMainWindow);
}

void Game::doMainLoop()
{
	SimpleTimer fpsTimer; // For calculating update delay and all
	int currentTime = fpsTimer.start();

	// Number of steps we need to do to be where we want to be
	int numberOfStepsToDo = (currentTime - mLastFrameTime)/mStepLength;

	doEvents();
	resetGraphics(); // Call before step if we want to do stuff in there

	if(mLastFrameTime != 0) // Make sure everything is good before moving stuff!
	{
		for(int i = 0; i < numberOfStepsToDo; i++)
			step(static_cast<float>(numberOfStepsToDo));
	}

	render();
	checkForErrors();

	mLastFrameTime = currentTime;

	int minTimePerFrame = 1000 / mMaxFramesPerSecond; // In miliseconds

	// If the frame took les ticks than the minimum, delay the next frame, virtually always does this.
	if(fpsTimer.getTicks() < minTimePerFrame)
	{
		SDL_Delay(minTimePerFrame - fpsTimer.getTicks()); // Delay the remaining time for the ticks per frame wanted
	}
}

// Public Interface //

// Initializes the game
// Returns false if it failed
bool Game::init()
{	
	Utils::LOGPRINT(std::string() + "Starting " + ENGINE_NAME + " v" + ENGINE_VERSION + "!");

	// SDL_INIT_AUDIO for SDL_mixer
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0)
	{
		// Failed
		Utils::CRASH_FROM_SDL("Unable to initialize SDL!");
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

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GRAPHICS_OPENGL_MAJOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GRAPHICS_OPENGL_MINOR_VERSION);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mMainWindow = SDL_CreateWindow(mName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mSize.x, mSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	
	if(!mMainWindow) // If the window failed to create, crash
	{
		Utils::CRASH_FROM_SDL("Unable to create window!");
		return false;
	}

	mMainContext = SDL_GL_CreateContext(mMainWindow); // Create OpenGL context!

	if(!mMainContext)
	{
		Utils::CRASH_FROM_SDL("Unable to create OpenGL context! This game requries OpenGL " +
			std::to_string(GRAPHICS_OPENGL_MAJOR_VERSION) + "." + std::to_string(GRAPHICS_OPENGL_MINOR_VERSION) +
			". Does your system support it? Try updating your graphics drivers!");
		return false;
	}

	SDL_GL_SetSwapInterval(1); // Kind of VSync?

	if(!gladLoadGL()) // Load OpenGL at runtime. I don't use SDL's loader, so no need to use gladLoadGLLoader().
	{
		Utils::CRASH("GLAD failed to load OpenGL!");
		return false;
	}

	// Output OpenGL version
	std::string glVersion;
	glVersion = (const char* )glGetString(GL_VERSION);
	glVersion = "Graphics: " + glVersion;
	Utils::LOGPRINT(glVersion);
	
	if(!checkCompability()) // This function logs errors
	{
		Utils::CRASH("System not compatible, cancelling init.");
		return false;
	}
	
	setupGraphics();
	checkForErrors();

	Utils::LOGPRINT("Initialization finished!");
	mInitialized = true;

	return true;
}

void Game::startMainLoop() // Starts the main loop
{
	if(mInitialized)
	{
		initMainLoop();

		while(!mQuitting) // While not quitting. mQuitting is set with quit()
		{
			doMainLoop();
		}

		cleanUp();
	} else
		Utils::CRASH("Game was not initialized before launching the main loop!");

	return; // Quit!
}

void Game::quit() // Call this when you want to quit to be clean
{
	mQuitting = true;
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
	
	// Resize the OpenGL viewport
	glViewport(0, 0, size.x, size.y);

	// Update camera
	mEntityManager.getGameCamera().setAspectRatio(calculateAspectRatio());
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

void Game::setGraphicsBackgroundColor(glm::vec3 color)
{
	mGraphicsBackgroundColor = color;
}

glm::vec3 Game::getGraphicsBackgroundColor()
{
	return mGraphicsBackgroundColor;
}

ResourceManager& Game::getResourceManager()
{
	return mResourceManager;
}

InputManager& Game::getInputManager()
{
	return mInputManager;
}

EntityManager& Game::getEntityManager()
{
	return mEntityManager;
}
