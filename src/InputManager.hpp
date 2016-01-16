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

#ifndef INPUT_MANAGER_HPP_
#define INPUT_MANAGER_HPP_

#include <SDL.h>

#include <map>
#include <cstddef> // For std::size_t
#include <vector>

class InputManager
{
private:
	typedef std::map<int, bool> sdlKeyMap; // Using a non-const key seems to be more compatible with different implementations
	typedef std::pair<int, bool> sdlKeyMapPair;

	sdlKeyMap mKeys;

public:
	typedef std::vector<int> keyVector;

	InputManager();
	~InputManager();
	void registerKey(int sdlKey);
	void registerKeys(const keyVector& keys);
	bool isKeyPressed(const int sdlKey);

	void updateKeyByEvent(SDL_Event event);
};

#endif /* INPUT_MANAGER_HPP_ */