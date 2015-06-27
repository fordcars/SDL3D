#include <InputHandler.h>

#include <SDL.h>
#include <HelperFunctions.h>

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
	std::pair<int, bool> key(sdlKey, false);
	mKeys.insert(key);
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
		HelperFunctions::warn("Key not found! Please register before using it");
		return false;
	}

	return got->second;
}