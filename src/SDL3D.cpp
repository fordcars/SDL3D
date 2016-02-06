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
#include <SDL3D.hpp> // <> because of CMake? Probably, <> is for the system include directory

#include <Game.hpp>
#include <Utils.hpp>

#include <stdio.h>
#include <memory> // For smart pointers. C++ libraries have no .h

int main(int argc, char **argv)
{
	// As annoying as it is to call this here, we have since we need to be
	// able to log as quick as possible (ex: constructing the Game).
	Utils::clearDataOutput();

	Game game;

	game.init();
	game.startMainLoop(); // Runs the game, returns when the game quits

    return 0;
}