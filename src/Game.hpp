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

#ifndef GAME_HPP_
#define GAME_HPP_

#include <GLAD/glad.h>
#include <SDL.h>

#include <ResourceManager.hpp>
#include <InputHandler.hpp>
#include <EntityManager.hpp>

#include <ShadedObject.hpp> // For testing
#include <Light.hpp> // For testing

class Game
{
private:
	std::string mGameName;
	std::string mLogFile;

	int mGameWidth;
	int mGameHeight;
	int mMinTicksPerFrame;
	
	int mLastFrameTime; // Time at last frame
	int mStepLength;

	bool mInitialized; // Set to true after initializing
	bool mQuitting; // If set to true, the game will quit at the end of the frame

	SDL_Window *mMainWindow;
	SDL_GLContext mMainContext; // OpenGl context

	ResourceManager mResourceManager; // On stack, calls its constructor by itself and cleans (deconstructs) itself like magic.
									  // But in this case, we need data from the user to create the resource manager, so we
									  // need a list initialization. See the Game constructor in Game.cpp

	InputHandler mInputHandler;

	EntityManager mEntityManager;

	void checkCompability();
	void initMainLoop();
	void cleanUp();

	void doEvents();
	void checkForErrors();

	void step();
	void render();
	void doMainLoop();

public:
	Game(const std::string& gameName, int width, int height, int frameRate, const std::string& resourceDir);
	~Game();
	void init();
	void startMainLoop();
	void quit();
};

#endif /* GAME_HPP_ */