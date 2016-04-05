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

#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <SDL.h>

#include <map>
#include <cstddef> // For std::size_t
#include <vector>

class InputManager
{
private:
	using sdlKeyMap = std::map<int, bool>; // Using a non-const key seems to be more compatible with different implementations
	using sdlKeyMapPair = std::pair<int, bool>;

	sdlKeyMap mKeys;

public:
	using keyVector = std::vector<int>;

	InputManager();
	~InputManager();
	void registerKey(int sdlKey);
	void registerKeys(const keyVector& keys);
	bool isKeyPressed(int sdlKey);

	void updateKeyByEvent(SDL_Event event);
};

#endif /* INPUT_MANAGER_HPP */