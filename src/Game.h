#ifndef GAME_H_
#define GAME_H_

#include <GL/glew.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>
#include <SDL.h>

#include <ResourceManager.h>
#include <InputHandler.h>
#include <Camera.h>

#include <Object.h> // For testing

class Game
{
private:
	std::string mGameName;
	std::string mLogFile;

	int mGameWidth;
	int mGameHeight;
	int mTicksPerFrame;

	SDL_Window *mMainWindow;
	SDL_GLContext mMainContext; // OpenGl context

	ResourceManager mResourceManager; // A reference, on stack, calls its constructor by itself and cleans (deconstructs) itself like magic.
									  // But in this case, we need data from the user to create the resource manager, so we
									  // need a list initialization. See the Game constructor in Game.cpp

	InputHandler mInputHandler; // A reference, when passed, does not copy the whole object: it only copies the reference.

	Camera mCamera;

	bool mQuitting; // If set to true, the game will quit at the end of the frame

	void preMainLoopInit();
	void update();
	void doEvents();
	void render();

	// Test
	Object *test;

public:
	Game(const std::string &gameName, int width, int height, int frameRate, const std::string &resourceDir);
	~Game();
	void init();
	void mainLoop();
	void quit();

	SDL_Window *getMainWindow();
	SDL_GLContext getMainContext();
	ResourceManager &getResourceManager(); // Returns a reference
	InputHandler &getInputHandler();
};

#endif /* GAME_H_ */