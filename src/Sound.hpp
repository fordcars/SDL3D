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

#ifndef SOUND_HPP
#define SOUND_HPP

#include "SDL_mixer.h"
#include <string>

class Sound
{
private:
	std::string mName;
	std::string mPath;
	int mType;

	// Static members (not functions) are not linked to any instance. Like a global!
	static int mInstanceCount; // Number of Sound instances

	// SDL_mixer needs pointers
	// Smells like C!
	Mix_Music* mMusicPointer; // Either this pointer or the chunk pointer is always defined (after load()), no need for checks!
	// The music that last played, might still be playing or not
	static Mix_Music* mLastPlayedMusic; // Only used for music, don't clear this!!!

	Mix_Chunk* mChunkPointer;
	int mChunkChannel; // Only used for chunks

	bool load();

public:
	Sound(const std::string& name, const std::string& path, int type);
	Sound(const Sound& other);
	~Sound();

	std::string getName();

	bool play(int numberOfLoops = 0);
	bool isPlaying();

	bool halt();
	void pause();
	bool isPaused();
	void resume();

	bool fadeIn(float fadeTime, int loops = 0);
	bool fadeOut(float fadeTime);

	void setVolume(int volume);
	int getVolume();
};

#endif /* SOUND_HPP */