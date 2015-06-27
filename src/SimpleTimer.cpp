#include <SimpleTimer.h>

#include <SDL.h>

SimpleTimer::SimpleTimer()
{
	startTicks = 0;
	endTicks = 0;
}

SimpleTimer::~SimpleTimer()
{
	// Do nothing
}

void SimpleTimer::start() // Can be called multiple times (resets timer)
{
	startTicks = SDL_GetTicks();
}

int SimpleTimer::getTicks()
{
	return SDL_GetTicks() - startTicks;
}