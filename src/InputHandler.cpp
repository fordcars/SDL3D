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

#include <InputHandler.hpp>

#include <SDL.h>
#include <string>
#include <Utils.hpp>

InputHandler::InputHandler()
{
	// Do nothing
}

InputHandler::~InputHandler()
{
	// Do nothing
}

void InputHandler::registerKey(int sdlKey) // Not const just in-case
{
	sdlKeyMapPair key(sdlKey, false);
	std::pair<sdlKeyMap::iterator, bool> newlyAddedPair = mKeys.insert(key);
	
	if(newlyAddedPair.second == false)
	{
		std::string error = "Key '" + std::to_string(sdlKey) + "' is already registered and can't be registered again!";
		Utils::crash(error, __LINE__, __FILE__);
	}
}

void InputHandler::registerKeys(int keys[], int length) // Not very safe
{
	for(int i=0; i<length; i++)
	{
		registerKey(keys[i]);
	}
}

void InputHandler::updateKeys(SDL_Event event) // Call each frame! Takes an event, and checks and updates keys.
{
	if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) // Make sure this is a keyboard event
	{
		SDL_KeyboardEvent keyEvent = event.key;

		// http://stackoverflow.com/questions/4844886/how-to-loop-through-a-c-map
		for(auto &it : mKeys) // Iterate through map. Using auto for C++11
		{
			if(keyEvent.keysym.sym == it.first)
			{
				keyEvent.type == SDL_KEYDOWN ? it.second = true : it.second = false; // If keydown, set to true
			}
		}
	}
}

bool InputHandler::keyPressed(const int sdlKey)
{
	sdlKeyMap::const_iterator got = mKeys.find(sdlKey);

	if(got==mKeys.end()) // Not found!
	{
		Utils::warn("Key not found! Please register before using it");
		return false;
	}

	return got->second;
}