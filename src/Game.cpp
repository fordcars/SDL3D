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

#include <Game.hpp>

#include <string> // No .h for c++

#include <math.h>
#include <memory> // For smart pointers

#include <glm/glm.hpp> // For matrices and all
#include <glad/glad.h> // For compability checks

#include <Utils.hpp>
#include <SimpleTimer.hpp> // For game loop
#include <Definitions.hpp>

// With the help of:
// https://www.opengl.org/wiki/Tutorial1:_Creating_a_Cross_Platform_OpenGL_3.2_Context_in_SDL_%28C_/_SDL%29
// http://glew.sourceforge.net/basic.html

Game::Game(const std::string& gameName, int width, int height, int maxFrameRate)
	: mResourceManager(getBasePath()) // Constructor
{
	mName = gameName; // Copy string

	mWidth = width;
	mHeight = height;
	mMinTimePerFrame = (int)(1000 / maxFrameRate); // Truncation

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
	GLuint vertexArrayID; // VAO - vertex aray object
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glEnable(GL_DEPTH_TEST);// Enable depth test (check if z is closer to the screen than last fragement's z)
	glDepthFunc(GL_LESS); // Accept the fragment closer to the camera

	// Cull triangles which normal is not towards the camera
	// If there are holes in the model because of this, click the "invert normals" button in your 3D modeler.
	glEnable(GL_CULL_FACE);
}

void Game::initMainLoop() // Initialize a few things before the main loop
{
	int keys[] = {SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_w, SDLK_a, SDLK_s, SDLK_d, SDLK_SPACE, SDLK_LSHIFT, SDLK_LCTRL};
	mInputManager.registerKeys(keys, 11);

	// Shaders
	mResourceManager.addShader("basic.v.glsl", "basic.f.glsl");
	mResourceManager.addShader("textured.v.glsl", "textured.f.glsl");
	mResourceManager.addShader("shaded.v.glsl", "shaded.f.glsl");

	// Textures
	mResourceManager.addTexture("test.bmp", BMP_TEXTURE);
	mResourceManager.addTexture("suzanne.dds", DDS_TEXTURE);
	mResourceManager.addTexture("building.dds", DDS_TEXTURE);
	mResourceManager.addTexture("minecraft.dds", DDS_TEXTURE);
	
	// Scripts
	mResourceManager.addScript(MAIN_SCRIPT_FILE);

	// Object groups
	mResourceManager.addObjectGeometryGroup("suzanne.obj");
	mResourceManager.addObjectGeometryGroup("building.obj");
	//mResourceManager.addObjectGeometryGroup("minecraft.obj");

	mEntityManager.getGameCamera().setAspectRatio((float)(mWidth/mHeight));
	mEntityManager.getGameCamera().setFieldOfView(70.0f); // Divided by: horizontal fov to vertical fov
	mEntityManager.getGameCamera().setPosition(glm::vec3(10.0f, 3.0f, 3.0f));

	mEntityManager.getGameCamera().setDirection(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)); // 0 for orientation

	EntityManager::objectPointer monkey(new ShadedObject(*mResourceManager.findObjectGeometryGroup("suzanne")->getObjectGeometries()[0], mResourceManager.findShader("shaded"), mResourceManager.findTexture("suzanne")));
	mEntityManager.addObject(monkey);

	mResourceManager.findScript(MAIN_SCRIPT_NAME)->bindInterface();
	mResourceManager.findScript(MAIN_SCRIPT_NAME)->run();

	/*// This is nuts
	for(std::size_t i=0; i<mResourceManager.findObjectGeometryGroup("minecraft")->getObjectGeometries().size(); i++)
	{
		EntityManager::objectPointer funTest(new ShadedObject(*mResourceManager.findObjectGeometryGroup("minecraft")->getObjectGeometries()[i], mResourceManager.findShader("shaded"), mResourceManager.findTexture("minecraft")));
		funTest->setScaling(glm::vec3(1.0f, 1.0f, 1.0f));
		mEntityManager.addObject(funTest);
	}*/

	EntityManager::lightPointer light(new Light(glm::vec3(4, 4, 4), glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), 60));
	mEntityManager.addLight(light);
}

void Game::cleanUp() // Cleans up everything. Call before quitting
{
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
		Utils::LOGPRINT("SDL message: " + message);
		SDL_ClearError();
	}
}

void Game::step() // Movement and all
{
	float speed = 0.01f;
	float rotateAngle = 0.01f;

	mEntityManager.getGameCamera().setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

	if(mInputManager.isKeyPressed(SDLK_LSHIFT))
	{
		speed *= 5;
	}

	if(mInputManager.isKeyPressed(SDLK_UP))
	{
		mEntityManager.getGameCamera().setVelocity(glm::vec3(mEntityManager.getGameCamera().getDirection()) * speed);
		//radius -= speed;
		//mEntityManager.getObjects()[0]->setVelocity(mEntityManager.getObjects()[0]->getVelocity()+glm::vec3(0.0005f, 0.0f, 0.0f));
		//mEntityManager.getObjects()[1]->setScaling(mEntityManager.getObjects()[1]->getScaling() + glm::vec3(0.01f, 0.01f, 0.01f));
	} else if(mInputManager.isKeyPressed(SDLK_DOWN))
	{
		mEntityManager.getGameCamera().setVelocity(glm::vec3(mEntityManager.getGameCamera().getDirection()) * -speed);
		//radius += speed;
		//mEntityManager.getObjects()[0]->setVelocity(mEntityManager.getObjects()[0]->getVelocity()-glm::vec3(0.0005f, 0.0f, 0.0f));
		//mEntityManager.getObjects()[1]->setScaling(mEntityManager.getObjects()[1]->getScaling() - glm::vec3(0.01f, 0.01f, 0.01f));
	}
	
	if(mInputManager.isKeyPressed(SDLK_LEFT))
	{
		mEntityManager.getGameCamera().setVelocity(mEntityManager.getGameCamera().getVelocity() +  glm::rotateY(glm::vec3(mEntityManager.getGameCamera().getDirection()) * speed, 1.5708f)  );
		//timeX += speed;
		//mEntityManager.getObjects()[1]->setRotation(mEntityManager.getObjects()[1]->getRotation()+glm::vec3(0.0f, 2.5f, 0.0f));
	} else if(mInputManager.isKeyPressed(SDLK_RIGHT))
	{
		mEntityManager.getGameCamera().setVelocity(mEntityManager.getGameCamera().getVelocity() +  glm::rotateY(glm::vec3(mEntityManager.getGameCamera().getDirection()) * speed, -1.5708f)  );
		//timeX -= speed;
		//mEntityManager.getObjects()[1]->setRotation(mEntityManager.getObjects()[1]->getRotation()-glm::vec3(0.0f, 2.5f, 0.0f));
	}

	if(mInputManager.isKeyPressed(SDLK_SPACE))
	{
		mEntityManager.getGameCamera().setPosition(mEntityManager.getGameCamera().getPosition() += glm::vec3(0.0f, speed, 0.0f));
	} else if(mInputManager.isKeyPressed(SDLK_LCTRL))
	{
		mEntityManager.getGameCamera().setPosition(mEntityManager.getGameCamera().getPosition() += glm::vec3(0.0f, -speed, 0.0f));
	}
	
	// Camera controls, needs work haha
	if(mInputManager.isKeyPressed(SDLK_w))
	{
		glm::vec4 direction(mEntityManager.getGameCamera().getDirection());

		// Get a 2D version and calculate the angle between them (think top-down, the other vector being a vector that goes forward)
		glm::vec2 forward2D(0.0f, 1.0f);
		glm::vec2 direction2D(direction.x, direction.z);

		// Get top down angle
		float angle = glm::orientedAngle(forward2D, glm::normalize(direction2D));

		// Rotate the 3D direction at that angle, so now it is facing forward
		glm::vec4 forwardDirection(glm::rotateY(direction, angle));
		forwardDirection = glm::rotateX(forwardDirection, -rotateAngle); // Make it rotate up

		mEntityManager.getGameCamera().setDirection(glm::rotateY(forwardDirection, -angle)); // Rotate it back
	} else if(mInputManager.isKeyPressed(SDLK_s))
	{
		glm::vec4 direction(mEntityManager.getGameCamera().getDirection());

		glm::vec2 forward2D(0.0f, 1.0f);
		glm::vec2 direction2D(direction.x, direction.z);

		float angle = glm::orientedAngle(forward2D, glm::normalize(direction2D));

		glm::vec4 forwardDirection(glm::rotateY(direction, angle));
		forwardDirection = glm::rotateX(forwardDirection, rotateAngle);

		mEntityManager.getGameCamera().setDirection(glm::rotateY(forwardDirection, -angle));
	}

	if(mInputManager.isKeyPressed(SDLK_a))
	{
		mEntityManager.getGameCamera().setDirection(glm::rotateY(mEntityManager.getGameCamera().getDirection(), rotateAngle));
	} else if(mInputManager.isKeyPressed(SDLK_d))
	{
		mEntityManager.getGameCamera().setDirection(glm::rotateY(mEntityManager.getGameCamera().getDirection(), -rotateAngle));
	}

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
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		Utils::CRASH("Unable to initialize SDL!");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mMainWindow = SDL_CreateWindow(mName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWidth, mHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	
	if(!mMainWindow) // If the window failed to create, crash
		Utils::CRASH_FROM_SDL("Unable to create window!");

	mMainContext = SDL_GL_CreateContext(mMainWindow); // Create OpenGL context!

	SDL_GL_SetSwapInterval(1); // Kind of VSync?

	if(!gladLoadGL()) // Load OpenGL at runtime. I don't use SDL's loader, so no need to use gladLoadGLLoader().
		Utils::CRASH("GLAD failed to load OpenGL!");

	// Output OpenGL version
	std::string glVersion;
	glVersion = (const char* )glGetString(GL_VERSION);
	glVersion = "Graphics: " + glVersion;
	Utils::LOGPRINT(glVersion);
	
	if(!checkCompability()) // Logs errors if it is not compatible
	{
		Utils::CRASH("System not compatible, cancelling init.");
		return;
	}
	
	setupGraphics();
	checkForErrors();

	Utils::LOGPRINT("Initialization finished!");
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
		Utils::CRASH("Game was not initialized before launching the main loop!");
	}

	return; // Quit!
}

void Game::quit() // Call this when you want to quit to be clean
{
	mQuitting = true;
}