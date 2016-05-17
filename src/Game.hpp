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

#ifndef GAME_HPP
#define GAME_HPP

#include <glad/glad.h>
#include <SDL.h>

#include <ResourceManager.hpp>
#include <InputManager.hpp>
#include <EntityManager.hpp>
#include <GraphicsManager.hpp>

#include <glm/glm.hpp>

class Game
{
private:
	std::string mName;
	std::string mLogFile;

	glm::ivec2 mSize;
	int mMaxFramesPerSecond;
	
	int mLastFrameTime; // Time at last frame
	int mStepLength;

	bool mInitialized; // Set to true after initializing
	bool mQuitting; // If set to true, the game will quit at the end of the frame

	// Pointers for SDL stuff needed
	SDL_Window* mMainWindow; // We might have multiple windows one day
	SDL_GLContext mMainContext; // OpenGl context

	ResourceManager mResourceManager; // On stack, calls its constructor by itself and cleans (deconstructs) itself like magic.
									  // But in this case, we need data from the user to create the resource manager, so we
									  // need a list initialization. See the Game constructor in Game.cpp.

	InputManager mInputManager;
	EntityManager mEntityManager;
	GraphicsManager mGraphicsManager;

	static std::string getBasePath();

	bool checkCompability();
	void setupGraphics();
	void initMainLoop();
	void cleanUp();

	void doEvents();
	void checkForErrors();

	float calculateAspectRatio();

	void step(float divider);
	void render();
	void doMainLoop();

public:
	Game();
	~Game();

	// Vital functions
	bool init();
	void startMainLoop();
	void quit();

	// Useful for scripting and other things
	void setName(const std::string& name);
	std::string getName();
	void setSize(glm::ivec2 size);
	glm::vec2 getSize();

	void setMaxFramesPerSecond(int maxFPS);
	void setMainWindowPosition(glm::ivec2 position);
	glm::ivec2 getMainWindowPosition();
	void reCenterMainWindow();

	ResourceManager& getResourceManager();
	InputManager& getInputManager();
	EntityManager& getEntityManager();
	GraphicsManager& getGraphicsManager();
};

#endif // GAME_HPP