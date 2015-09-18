// Copyright 2015 Carl Hewett

// This file is part of SDL3D.

// SDL3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// SDL3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.

#include <Game.h>

#include <string> // No .h for c++
#include <iostream>
#include <fstream>
#include <math.h>
#include <memory> // For smart pointers

#include <glm/glm.hpp> // For matrices and all
#include <HelperFunctions.h>
#include <SimpleTimer.h> // For game loop
#include <Definitions.h>

// With the help of:
// https://www.opengl.org/wiki/Tutorial1:_Creating_a_Cross_Platform_OpenGL_3.2_Context_in_SDL_%28C_/_SDL%29
// http://glew.sourceforge.net/basic.html

using namespace HelperFunctions;

Game::Game(const std::string& gameName, int width, int height, int frameRate, const std::string& resourceDir)
	: mResourceManager(resourceDir) // Constructor
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

void Game::checkCompability() // Checks if the game will work on the user's setup
{
	std::string extensions[] = {"GL_EXT_texture_compression_s3tc"};
	int numberOfExtensions = 1;

	GLint numExt;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);

	bool isCompatible = true; // True if the game is compatible with the setup (after we are done checking the setup here)
	
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
			warn("The extension " + extensions[i] + " has not been found on your system.");
			isCompatible = false; // Not compatible!
			// Don't break, let the user know what other extensions are not found (if it is the case)
		}
	}

	if(isCompatible)
		info("Your system seems to be compatible with the game!");
	else
		crash("Your system is not compatible with the game. Please take a look at the generated warnings.");
}

void Game::preMainLoopInit() // A few initializations before the main game loop
{
	int keys[] = {SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_SPACE};
	mInputHandler.registerKeys(keys, 5);

	GLuint vertexArrayID; // VAO - vertex aray object
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glEnable(GL_DEPTH_TEST);// Enable depth test (check if z is closer to the screen than last fragement's z)
	glDepthFunc(GL_LESS); // Accept the fragment closer to the camera

	// Cull triangles which normal is not towards the camera
	// If there are holes in the model because of this, click the "invert normals" button in your 3D modeler.
	glEnable(GL_CULL_FACE);

	// Shaders
	mResourceManager.addShader("Textured", "textured.v.glsl", "textured.f.glsl");
	mResourceManager.addTexture("Test.bmp", BMP_TEXTURE);
	mResourceManager.addTexture("TestDDS", "Test.dds", DDS_TEXTURE);

	const std::string uniforms[] = {"MVP", "textureSampler", "textureType"};
	mResourceManager.findShader("Textured")->addUniforms(uniforms, 3);

	// Test (Game.h, render() and here)
	GLfloatArray vertices = {
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

	// Two UV coordinatesfor each vertex. They were created with Blender. You'll learn shortly how to do this yourself.
	GLfloatArray UVCoords = {
		0.000059f, 1.0f-0.000004f,
		0.000103f, 1.0f-0.336048f,
		0.335973f, 1.0f-0.335903f,
		1.000023f, 1.0f-0.000013f,
		0.667979f, 1.0f-0.335851f,
		0.999958f, 1.0f-0.336064f,
		0.667979f, 1.0f-0.335851f,
		0.336024f, 1.0f-0.671877f,
		0.667969f, 1.0f-0.671889f,
		1.000023f, 1.0f-0.000013f,
		0.668104f, 1.0f-0.000013f,
		0.667979f, 1.0f-0.335851f,
		0.000059f, 1.0f-0.000004f,
		0.335973f, 1.0f-0.335903f,
		0.336098f, 1.0f-0.000071f,
		0.667979f, 1.0f-0.335851f,
		0.335973f, 1.0f-0.335903f,
		0.336024f, 1.0f-0.671877f,
		1.000004f, 1.0f-0.671847f,
		0.999958f, 1.0f-0.336064f,
		0.667979f, 1.0f-0.335851f,
		0.668104f, 1.0f-0.000013f,
		0.335973f, 1.0f-0.335903f,
		0.667979f, 1.0f-0.335851f,
		0.335973f, 1.0f-0.335903f,
		0.668104f, 1.0f-0.000013f,
		0.336098f, 1.0f-0.000071f,
		0.000103f, 1.0f-0.336048f,
		0.000004f, 1.0f-0.671870f,
		0.336024f, 1.0f-0.671877f,
		0.000103f, 1.0f-0.336048f,
		0.336024f, 1.0f-0.671877f,
		0.335973f, 1.0f-0.335903f,
		0.667969f, 1.0f-0.671889f,
		1.000004f, 1.0f-0.671847f,
		0.667979f, 1.0f-0.335851f
	};

	mCamera.setAspectRatio((float)(mGameWidth/mGameHeight));
	mCamera.setFieldOfView(90);
	mCamera.setPos(glm::vec3(4.0f, 3.0f, 3.0f));

	test = new TexturedObject(vertices, 12 * 3, UVCoords, mResourceManager.findShader("Textured"), mResourceManager.findTexture("Test")); // Obviously a test
}

void Game::cleanUp() // Cleans up everything. Call before quitting
{
	SDL_GL_DeleteContext(mMainContext);
	SDL_DestroyWindow(mMainWindow);
	SDL_Quit();
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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear both color buffers and depth (z-indexes) buffers to push a clean buffer when done

	glm::mat4 model = glm::mat4(1.0f); // Normally would have rotation/translation/scaling
	glm::mat4 MVP = mCamera.getProjectionMatrix() * mCamera.getViewMatrix() * model; // Yey
	
	test->render(MVP);

	SDL_GL_SwapWindow(mMainWindow);
}

void Game::checkForErrors() // Call each frame for safety. Do not call after deleting the OpenGL context
{
	GLenum err = glGetError();

	if(err!=GL_NO_ERROR) // There is an error
	{
		if(err == GL_INVALID_ENUM)
			warn("OpenGL error: GL_INVALID_ENUM");
		else if(err == GL_INVALID_VALUE)
			warn("OpenGL error: GL_INVALID_VALUE");
		else if(err == GL_INVALID_OPERATION)
			warn("OpenGL error: GL_INVALID_OPERATION");
		else if(err == GL_STACK_OVERFLOW)
			warn("OpenGL error: GL_STACK_OVERFLOW");
		else if(err == GL_STACK_UNDERFLOW)
			warn("OpenGL error: GL_STACK_UNDERFLOW");
		else if(err == GL_OUT_OF_MEMORY)
			warn("OpenGL error: GL_OUT_OF_MEMORY");
	}
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
	checkForErrors();

	if(fpsTimer.getTicks() < mTicksPerFrame) // Frame was too quick!
	{
		SDL_Delay(mTicksPerFrame - fpsTimer.getTicks()); // Delay the remaining time for the ticks per frame wanted
	}
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

	if(!gladLoadGL()) // Load OpenGL at runtime. I don't use SDL's loader, so no need to use gladLoadGLLoader().
		crash("GLAD failed to load OpenGL!");

	// Output OpenGL version
	std::string glVersion;
	glVersion = (const char *)glGetString(GL_VERSION);
	glVersion = "Graphics: " + glVersion;
	info(glVersion);
	
	checkCompability();
}

void Game::mainLoop() // Starts the main loop
{
	preMainLoopInit();

	while(!mQuitting) // While not quitting. mQuitting is set with quit()
	{
		update();
	}

	cleanUp();
	info("Game quit successfully.");

	return; // Quit!
}

void Game::quit() // Call this when you want to quit to be clean
{
	mQuitting = true;
}