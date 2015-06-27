#ifndef INPUTHANDLER_H_
#define INPUTHANDLER_H_

#include <SDL.h>
#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <unordered_map>

class InputHandler
{
	typedef std::unordered_map<int, bool> sdlKeyMap; // Using a non-const key seems to be more compatible with different implementations
private:
	sdlKeyMap mKeys;

public:
	InputHandler();
	~InputHandler();
	void registerKey(int sdlKey);
	void registerKeys(int keys[], int length);
	void updateKeys(SDL_Event event);
	bool keyPressed(const int sdlKey);
};

#endif /* INPUTHANDLER_H_ */