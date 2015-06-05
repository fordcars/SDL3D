#include <SDL.h> // <> because of CMake? Probably, <> is for the system include directory
#include <stdio.h>
#include <memory> // C++ libraries have no .h

#include <Game.h>
#include <SDL3D.h>

typedef std::unique_ptr<Game> gameType;

// Note: smart pointers use the heap. Smart pointers deallocates the memory automatically
// The auto 'data type' guess the data type, useful for templates and stuff

int main(int argc, char **argv)
{
	gameType game = gameType(new Game("SDL3d", 800, 600, "resource/")); // Smart pointer, calls deconstructor when the scop ends
	game->init();
	
	game->getResourceManager()->addShader("Basic", "basic.v.glsl", "basic.f.glsl");
	game->getResourceManager()->findShader("Basic");

    return 0;
}