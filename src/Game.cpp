#include <HelperFunctions.h>
#include <Game.h>

#include <string> // No .h for c++
#include <iostream>
#include <fstream>

// With the help of:
// https://www.opengl.org/wiki/Tutorial1:_Creating_a_Cross_Platform_OpenGL_3.2_Context_in_SDL_%28C_/_SDL%29
// http://glew.sourceforge.net/basic.html

using namespace HelperFunctions;

Game::Game(const char *gameName, int width, int height, const char *resourceDir) // Constructor
{
	mGameName = gameName; // Since std::string casting thingy is implicit, it converts automatically
	mResourceDir = resourceDir;

	mGameWidth = width;
	mGameHeight = height;

	mResourceManager = new ResourceManager(mResourceDir.c_str());
}

Game::~Game() // Deconstructor
{
	delete mResourceManager;
	quit();
}

void Game::init()
{
	HelperFunctions::clearDataOutput();

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		crash("Unable to initialize SDL");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mMainWindow = SDL_CreateWindow(mGameName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mGameWidth, mGameHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	
	if(!mMainWindow) // If the window failed to create, crash
		crash("Unable to create window!");

	checkSDLError(__LINE__);

	mMainContext = SDL_GL_CreateContext(mMainWindow); // Create OpenGL context!
	checkSDLError(__LINE__);

	SDL_GL_SetSwapInterval(1); // Kind of VSync?

	GLenum err = glewInit();
	if(err!=GLEW_OK)
	{
		std::string glewError;
		glewError = (const char *)glewGetErrorString(err);
		crash(glewError.c_str());
	}

	// Output OpenGL version
	std::string glVersion;
	glVersion = (const char *)glGetString(GL_VERSION);
	glVersion = "Graphics: " + glVersion;
	info(glVersion.c_str());
}

void Game::quit()
{
	SDL_GL_DeleteContext(mMainContext);
	SDL_DestroyWindow(mMainWindow);
	SDL_Quit();

	info("Game quit successfully.");
}

SDL_Window *Game::getMainWindow()
{
	return mMainWindow;
}

SDL_GLContext Game::getMainContext()
{
	return mMainContext;
}

ResourceManager *Game::getResourceManager()
{
	return mResourceManager;
}