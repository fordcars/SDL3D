#include <SDL.h> // <> because of CMake? Probably, <> is for the system include directory
#include <stdio.h>
#include <memory> // For smart pointers. C++ libraries have no .h

#include <Game.h>
#include <SDL3D.h>

typedef std::unique_ptr<Game> gamePointer;

// Note: smart pointers use the heap. Smart pointers deallocates the memory automatically
// The auto 'data type' guess the data type, useful for templates and stuff

int main(int argc, char **argv)
{
	gamePointer game(new Game("SDL3D", 800, 600, 60, "resource/")); // Smart pointer, calls deconstructor when the scope ends (this is an example of smart pointer)
																		// It would of been better to use a reference instead (no pointer).

	game->init();
	game->mainLoop(); // Runs the game, returns when the game quits

    return 0;
}