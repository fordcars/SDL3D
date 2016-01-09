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

#include <SimpleTimer.hpp>

#include <SDL.h>

SimpleTimer::SimpleTimer()
{
	mStartTicks = 0;
	mEndTicks = 0;
}

SimpleTimer::~SimpleTimer()
{
	// Do nothing
}

int SimpleTimer::start() // Can be called multiple times (resets timer)
{
	mStartTicks = SDL_GetTicks(); // SDL_GetTicks() returns the number of miliseconds since SDL started
	return mStartTicks;
}

int SimpleTimer::getTicks()
{
	return SDL_GetTicks() - mStartTicks;
}