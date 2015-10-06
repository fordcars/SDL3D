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

#ifndef GAME_H_
#define GAME_H_

#include <GLAD/glad.h>
#include <SDL.h>

#include <ResourceManager.h>
#include <InputHandler.h>
#include <Camera.h>

#include <Object.h> // For testing
#include <TexturedObject.h> // For testing

class Game
{
private:
	std::string mGameName;
	std::string mLogFile;

	int mGameWidth;
	int mGameHeight;
	int mMinTicksPerFrame;
	
	int mLastFrameTime; // Time at last frame
	const int mGameSpeedDivider; // Delta is divided by this. Setting this to 16 makes delta 1 (per frame) at 60fps.

	SDL_Window *mMainWindow;
	SDL_GLContext mMainContext; // OpenGl context

	ResourceManager mResourceManager; // On stack, calls its constructor by itself and cleans (deconstructs) itself like magic.
									  // But in this case, we need data from the user to create the resource manager, so we
									  // need a list initialization. See the Game constructor in Game.cpp

	InputHandler mInputHandler; // A reference, when passed, does not copy the whole object: it only copies the reference.

	Camera mCamera;

	bool mQuitting; // If set to true, the game will quit at the end of the frame

	void checkCompability();
	void preMainLoopInit();
	void cleanUp();

	void doEvents();
	void render();
	void checkForErrors();
	void update();

	// Test
	TexturedObject *test;

public:
	Game(const std::string& gameName, int width, int height, int frameRate, const std::string& resourceDir);
	~Game();
	void init();
	void mainLoop();
	void quit();
};

#endif /* GAME_H_ */