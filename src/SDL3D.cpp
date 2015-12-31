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

#include <SDL.h> // <> because of CMake? Probably, <> is for the system include directory
#include <stdio.h>
#include <memory> // For smart pointers. C++ libraries have no .h

#include <Game.hpp>
#include <SDL3D.hpp>

typedef std::unique_ptr<Game> gamePointer;

// Note: smart pointers use the heap. Smart pointers deallocates the memory automatically
// The auto 'data type' guess the data type, useful for templates and stuff

int main(int argc, char **argv)
{
	gamePointer game(new Game("SDL3D", 1024, 720, 60, "resource/")); // Smart pointer, calls deconstructor when the scope ends (this is an example of smart pointer)
																		// It would of been better to use a reference instead (no pointer).

	game->init();
	game->startMainLoop(); // Runs the game, returns when the game quits

    return 0;
}