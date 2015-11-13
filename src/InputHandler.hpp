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

#ifndef INPUTHANDLER_HPP_
#define INPUTHANDLER_HPP_

#include <SDL.h>
#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <unordered_map>

class InputHandler
{
private:
	typedef std::unordered_map<int, bool> sdlKeyMap; // Using a non-const key seems to be more compatible with different implementations
	typedef std::pair<int, bool> sdlKeyMapPair;

	sdlKeyMap mKeys;

public:
	InputHandler();
	~InputHandler();
	void registerKey(int sdlKey);
	void registerKeys(int keys[], int length);
	void updateKeys(SDL_Event event);
	bool keyPressed(const int sdlKey);
};

#endif /* INPUTHANDLER_HPP_ */