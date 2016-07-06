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

#include "ResourceManager.hpp"
#include "InputManager.hpp"
#include "EntityManager.hpp"
#include "GraphicsManager.hpp"

#include "SDL.h"

#include "glm/glm.hpp"

#include <memory> // For smart pointers
#include <string>

class Game
{
public:
	using resourceManagerPointer = std::shared_ptr<ResourceManager>;
	using inputManagerPointer = std::shared_ptr<InputManager>;
	using entityManagerPointer = std::shared_ptr<EntityManager>;
	using graphicsManagerPointer = std::shared_ptr<GraphicsManager>;

private:
	std::string mName;

	bool mQuitting; // Set to true (through quit()) to quit the game
	glm::ivec2 mSize;
	int mMaxFramesPerSecond;
	
	int mLastFrameTime; // Time at last frame
	int mStepLength;

	// Pointers for SDL stuff needed
	SDL_Window* mMainWindow; // We might have multiple windows one day
	SDL_GLContext mMainContext; // OpenGl context

	// These are pointers since we need to create them after some initialization
	resourceManagerPointer mResourceManager; // On stack, calls its constructor by itself and cleans (deconstructs) itself like magic.
									  // But in this case, we need data from the user to create the resource manager, so we
									  // need a list initialization. See the Game constructor in Game.cpp.

	inputManagerPointer mInputManager;
	entityManagerPointer mEntityManager;
	graphicsManagerPointer mGraphicsManager;

	bool init();
	bool initSDL();
	bool initContext();

	bool checkCompability();
	bool checkForErrors();
	std::string getBasePath();

	void doMainLoop();

	void doEvents();

	void step(float divider);
	void render();

	void cleanup();

public:
	Game();
	~Game();

	void startMainLoop();
	void quit();

	float getAspectRatio();

	// Useful for scripting and other things
	void setName(const std::string& name);
	std::string getName();
	void setSize(glm::ivec2 size);
	glm::vec2 getSize();

	void setMaxFramesPerSecond(int maxFPS);
	void setMainWindowPosition(glm::ivec2 position);
	glm::ivec2 getMainWindowPosition();
	void reCenterMainWindow();

	resourceManagerPointer getResourceManager();
	inputManagerPointer getInputManager();
	entityManagerPointer getEntityManager();
	graphicsManagerPointer getGraphicsManager();
};

#endif // GAME_HPP