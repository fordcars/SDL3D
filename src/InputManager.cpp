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

#include <InputManager.hpp>

#include <SDL.h>
#include <string>
#include <Utils.hpp>

InputManager::InputManager()
{
	// Do nothing
}

InputManager::~InputManager()
{
	// Do nothing
}

void InputManager::registerKey(int sdlKey) // Not const just in-case
{
	sdlKeyMapPair key(sdlKey, false);
	std::pair<sdlKeyMap::iterator, bool> newlyAddedPair = mKeys.insert(key);
	
	if(newlyAddedPair.second == false)
	{
		std::string error = "Key '" + std::to_string(sdlKey) + "' is already registered and can't be registered again!";
		Utils::CRASH(error);
	}
}

void InputManager::registerKeys(const keyVector& keys)
{
	for(const auto& key : keys) // Key is the actual value, not an iterator
		registerKey(key);
}

bool InputManager::isKeyPressed(int sdlKey)
{
	sdlKeyMap::const_iterator got = mKeys.find(sdlKey);

	if(got==mKeys.end()) // Not found!
	{
		Utils::WARN("Key not found! Please register before using it");
		return false;
	}

	return got->second;
}

// Call each frame! Takes an event, and checks and updates keys.
// One event can only talk about one key, so iterate through the SDL events and call this each time
void InputManager::updateKeyByEvent(SDL_Event event)
{
	if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) // Make sure this is a keyboard event
	{
		SDL_KeyboardEvent keyEvent = event.key;
		sdlKeyMap::iterator sdlKeyIt = mKeys.find(keyEvent.keysym.sym); // Find the key in the map

		if(sdlKeyIt != mKeys.end()) // If it is there, the found registered key is being pressed
		{
			keyEvent.type == SDL_KEYDOWN ? sdlKeyIt->second = true : sdlKeyIt->second = false; // If keydown, set to true
		}
	}
}