//// Copyright 2015 Carl Hewett
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

#include <Game.hpp>

#include <string> // No .h for c++

#include <math.h>
#include <memory> // For smart pointers

#include <glm/glm.hpp> // For matrices and all
#include <Utils.hpp>
#include <SimpleTimer.hpp> // For game loop
#include <Definitions.hpp>

// With the help of:
// https://www.opengl.org/wiki/Tutorial1:_Creating_a_Cross_Platform_OpenGL_3.2_Context_in_SDL_%28C_/_SDL%29
// http://glew.sourceforge.net/basic.html

Game::Game(const std::string& gameName, int width, int height, int maxFrameRate, const std::string& resourceDir)
	: mResourceManager(resourceDir) // Constructor
{
	mGameName = gameName; // Copy string

	mGameWidth = width;
	mGameHeight = height;
	mMinTimePerFrame = (int)(1000 / maxFrameRate); // Trucation

	mLastFrameTime = 0;

	// This is the length of a step, used for movement and everything, in ms. 8 ms makes 120 steps per second.
	// If the time of one frame is smaller than this value (ex: faster screens in the future), the game will slow down.
	mStepLength = 8;

	mInitialized = false;
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
			Utils::warn("The extension " + extensions[i] + " has not been found on your system.");
			isCompatible = false; // Not compatible!
			// Don't break, let the user know what other extensions are not found (if it is the case)
		}
	}

	if(isCompatible)
		Utils::logprint("Your system seems to be compatible with the game!");
	else
		Utils::crash("Your system is not compatible with the game. Please take a look at the generated warnings.", __LINE__, __FILE__);
}

void Game::initMainLoop() // Initialize a few things before the main loop
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
	mResourceManager.addShader("shaded", "shaded.v.glsl", "shaded.f.glsl");
	mResourceManager.addTexture("test.bmp", BMP_TEXTURE);
	mResourceManager.addTexture("suzanne.dds", DDS_TEXTURE);
	mResourceManager.addTexture("building.dds", DDS_TEXTURE);
	
	// Test (Game.h, render() and here)
	mResourceManager.addObjectGeometry("suzanne.obj");
	mResourceManager.addObjectGeometry("building.obj");

	mEntityManager.getGameCamera().setAspectRatio((float)(mGameWidth/mGameHeight));
	mEntityManager.getGameCamera().setFieldOfView(70.0f); // Divided by: horizontal fov to vertical fov
	mEntityManager.getGameCamera().setPosition(glm::vec3(10.0f, 3.0f, 3.0f));

	EntityManager::objectPointer monkey(new ShadedObject(mResourceManager.findObjectGeometry("suzanne"), mResourceManager.findShader("shaded"), mResourceManager.findTexture("suzanne")));
	mEntityManager.addObject(monkey);

	EntityManager::objectPointer building(new ShadedObject(mResourceManager.findObjectGeometry("building"), mResourceManager.findShader("shaded"), mResourceManager.findTexture("building")));
	mEntityManager.addObject(building);

	EntityManager::lightPointer light(new Light(glm::vec3(4, 4, 4), glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), 60));
	mEntityManager.addLight(light);

	//mEntityManager.getObjects()[0]->setVelocity(glm::vec3(0.05f, 0.0f, 0.0f));
	mEntityManager.getObjects()[0]->setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
}

void Game::cleanUp() // Cleans up everything. Call before quitting
{
	SDL_GL_DeleteContext(mMainContext);
	SDL_DestroyWindow(mMainWindow);
	SDL_Quit();

	Utils::logprint("Game quit successfully.");
	Utils::closeLogFile();
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

void Game::checkForErrors() // Call each frame for safety. Do not call after deleting the OpenGL context.
{
	for(int i=0; i<1000; i++) // Because meh, I don't like infinite loops
	{
		GLenum err = glGetError();
		
		if(err!=GL_NO_ERROR) // There is an error
		{
			if(err == GL_INVALID_ENUM)
				Utils::warn("OpenGL error: GL_INVALID_ENUM");
			else if(err == GL_INVALID_VALUE)
				Utils::warn("OpenGL error: GL_INVALID_VALUE");
			else if(err == GL_INVALID_OPERATION)
				Utils::warn("OpenGL error: GL_INVALID_OPERATION");
			else if(err == GL_STACK_OVERFLOW)
				Utils::warn("OpenGL error: GL_STACK_OVERFLOW");
			else if(err == GL_STACK_UNDERFLOW)
				Utils::warn("OpenGL error: GL_STACK_UNDERFLOW");
			else if(err == GL_OUT_OF_MEMORY)
				Utils::warn("OpenGL error: GL_OUT_OF_MEMORY");
		} else // No (more) errors!
		{
			return;
		}
	}

	// Over 1000 errors, is this possible?
	Utils::warn("Over 1000 OpenGL errors?!");
}


float timeX = 0;  //DEBUUUUUUUUUUUUUUUGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
float radius = 1; //DEBUUUUUUUUUUUUUUUGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
void Game::step() // Movement and all
{
	float speed = 0.05f;

	if(mInputHandler.keyPressed(SDLK_UP))
	{
		//radius -= speed;
		mEntityManager.getObjects()[0]->setVelocity(mEntityManager.getObjects()[0]->getVelocity()+glm::vec3(0.0005f, 0.0f, 0.0f));
		//mEntityManager.getObjects()[0]->setScaling(mEntityManager.getObjects()[0]->getScaling() + glm::vec3(0.01f, 0.01f, 0.01f));
	} else if(mInputHandler.keyPressed(SDLK_DOWN))
	{
		//radius += speed;
		mEntityManager.getObjects()[0]->setVelocity(mEntityManager.getObjects()[0]->getVelocity()-glm::vec3(0.0005f, 0.0f, 0.0f));
		//mEntityManager.getObjects()[0]->setScaling(mEntityManager.getObjects()[0]->getScaling() - glm::vec3(0.01f, 0.01f, 0.01f));
	} else if(mInputHandler.keyPressed(SDLK_LEFT))
	{
		//timeX += speed;
		mEntityManager.getObjects()[0]->setRotation(mEntityManager.getObjects()[0]->getRotation()+glm::vec3(0.0f, 2.5f, 0.0f));
	} else if(mInputHandler.keyPressed(SDLK_RIGHT))
	{
		//timeX -= speed;
		mEntityManager.getObjects()[0]->setRotation(mEntityManager.getObjects()[0]->getRotation()-glm::vec3(0.0f, 2.5f, 0.0f));
	}

	//glm::vec3 position(radius * cos(timeX), mCamera.getPosition().y, radius * sin(timeX));
	//mCamera.setPosition(position);

	mEntityManager.step();
}

void Game::render()
{
	glClearColor(0.1f, 0.1f, 1.0f, 1.0f); // Set clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear both color buffers and depth (z-indexes) buffers to push a clean buffer when done

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

	if(mLastFrameTime != 0) // Make sure everything is good before moving stuff!
	{
		for(int i = 0; i < numberOfStepsToDo; i++)
			step();
	}

	render();
	checkForErrors();

	mLastFrameTime = currentTime;

	// If the frame took les ticks than the minimum, delay the next frame, virtually always does this.
	if(fpsTimer.getTicks() < mMinTimePerFrame)
	{
		SDL_Delay(mMinTimePerFrame - fpsTimer.getTicks()); // Delay the remaining time for the ticks per frame wanted
	}
}

// Public Interface //

void Game::init() // Starts the game
{
	Utils::clearDataOutput();
	
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		Utils::crash("Unable to initialize SDL", __LINE__, __FILE__);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mMainWindow = SDL_CreateWindow(mGameName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mGameWidth, mGameHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	
	if(!mMainWindow) // If the window failed to create, crash
		Utils::crash("Unable to create window!", __LINE__, __FILE__);

	Utils::checkSDLError(__LINE__, __FILE__);

	mMainContext = SDL_GL_CreateContext(mMainWindow); // Create OpenGL context!
	Utils::checkSDLError(__LINE__, __FILE__);

	SDL_GL_SetSwapInterval(1); // Kind of VSync?

	if(!gladLoadGL()) // Load OpenGL at runtime. I don't use SDL's loader, so no need to use gladLoadGLLoader().
		Utils::crash("GLAD failed to load OpenGL!", __LINE__, __FILE__);

	// Output OpenGL version
	std::string glVersion;
	glVersion = (const char *)glGetString(GL_VERSION);
	glVersion = "Graphics: " + glVersion;
	Utils::logprint(glVersion);
	
	checkCompability();

	Utils::logprint("Initialization was a success!");
	mInitialized = true;
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
	{
		Utils::clearDataOutput(); // Just to make sure the log is empty
		Utils::crash("Game was not initialized before launching the main loop!", __LINE__, __FILE__);
	}

	return; // Quit!
}

void Game::quit() // Call this when you want to quit to be clean
{
	mQuitting = true;
}