#include <Game.h>

#include <string> // No .h for c++
#include <iostream>
#include <fstream>
#include <math.h>

#include <HelperFunctions.h>
#include <SimpleTimer.h> // For game loop

// With the help of:
// https://www.opengl.org/wiki/Tutorial1:_Creating_a_Cross_Platform_OpenGL_3.2_Context_in_SDL_%28C_/_SDL%29
// http://glew.sourceforge.net/basic.html

using namespace HelperFunctions;

Game::Game(const std::string &gameName, int width, int height, int frameRate, const std::string &resourceDir) : mResourceManager(resourceDir) // Constructor
{
	mGameName = gameName; // Copy string

	mGameWidth = width;
	mGameHeight = height;
	mTicksPerFrame = (int)(1000 / frameRate); // Trucation

	mQuitting = false;
}

Game::~Game() // Deconstructor
{
	quit();
}

void Game::preMainLoopInit() // A few initializations before the main game loop
{
	int keys[] = {SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT};
	mInputHandler.registerKeys(keys, 4);

	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	mResourceManager.addShader("Basic", "basic.v.glsl", "basic.f.glsl");
	mResourceManager.findShader("Basic");

	// Uniforms
	mResourceManager.addUniform("MVP", "Basic");

	// Test
	vertexArray vertices = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};

	mCamera.setAspectRatio(800/600);
	mCamera.setFieldOfView(90);
	mCamera.setPos(glm::vec3(4.0f, 3.0f, 3.0f));

	test = new Object(vertices, 12 * 3); // Obviously a test
}

void Game::update()
{
	SimpleTimer fpsTimer; // Frame rate (a reference)
	fpsTimer.start();

	doEvents();

	float speed = 0.5;

	if(mInputHandler.keyPressed(SDLK_UP))
	{
		mCamera.translate(glm::vec3(0, speed, 0));
	} else if(mInputHandler.keyPressed(SDLK_DOWN))
	{

		mCamera.translate(glm::vec3(0, -speed, 0));
	} else if(mInputHandler.keyPressed(SDLK_LEFT))
	{
		mCamera.translate(glm::vec3(-speed, 0, 0));
	} else if(mInputHandler.keyPressed(SDLK_RIGHT))
	{
		mCamera.translate(glm::vec3(speed, 0, 0));
	}


	render();

	if(fpsTimer.getTicks() < mTicksPerFrame) // Frame was too quick!
	{
		SDL_Delay(mTicksPerFrame - fpsTimer.getTicks()); // Delay the remaining time for the ticks per frame wanted
	}
}

void Game::doEvents()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		mInputHandler.updateKeys(event);

		if(event.type == SDL_QUIT)
			quit();
	}
}

void Game::render()
{
	mCamera.updateMatrices();
	glm::mat4 model = glm::mat4(1.0f); // Normally would have rotation/translation/scaling
	glm::mat4 MVP = mCamera.getProjectionMatrix() * mCamera.getViewMatrix() * model; // Yey

	glUniformMatrix4fv(mResourceManager.findUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(mResourceManager.findShader("Basic"));

	test->render();

	SDL_GL_SwapWindow(mMainWindow);
}

void Game::init() // Starts the game
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

	glewExperimental = GL_TRUE; // Enable experimental GLEW for added functions
	GLenum err = glewInit(); // Init GLEW
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
	info(glVersion);
}

void Game::mainLoop() // Starts the main loop
{
	preMainLoopInit();

	while(!mQuitting) // While not quitting. mQuitting is set with quit()
	{
		update();
	}
}

void Game::quit() // Call this when you want to quit to be clean
{
	if(!mQuitting) // If not already quitting
	{
		SDL_GL_DeleteContext(mMainContext);
		SDL_DestroyWindow(mMainWindow);
		SDL_Quit();

		mQuitting = true;
		info("Game quit successfully.");
	}
}

SDL_Window *Game::getMainWindow()
{
	return mMainWindow;
}

SDL_GLContext Game::getMainContext()
{
	return mMainContext;
}

ResourceManager &Game::getResourceManager() // Returns a reference
{
	return mResourceManager;
}

InputHandler &Game::getInputHandler()
{
	return mInputHandler;
}