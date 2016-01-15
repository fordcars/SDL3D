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

// A wrapper class that generalizes the SDL_mixer API
// Only one music can play at once!

#include <Sound.hpp>

#include <Utils.hpp>
#include <Definitions.hpp>

// Done once
// Static member initialized
int Sound::mInstanceCount = 0;

Sound::Sound(const std::string& name, const std::string& path, int type)
{
	mName = name;
	mPath = path;
	mType = type;

	// Set all pointers to NULL, C++11 style!
	mMusicPointer = nullptr;
	mChunkPointer = nullptr;
	mChunkChannel = 0;

	mInstanceCount++; // One more instance created!

	load();
}

// Heavy! Reloads the sound.
Sound::Sound(const Sound& other)
{
	// Looks like the best (and only?) way to copy a sound is to reload it
	mName = other.mName;
	mPath = other.mPath;
	mType = other.mType;

	mMusicPointer = nullptr;
	mChunkPointer = nullptr;
	mChunkChannel = 0; // Different channel, of course!

	mInstanceCount++;

	load();
}

Sound::~Sound()
{
	switch(mType)
	{
	case SOUND_MUSIC:
		Mix_FreeMusic(mMusicPointer);
		break;

	case SOUND_CHUNK:
		Mix_FreeChunk(mChunkPointer);
		break;

	default:
		// Errors should already of have been taken care of, no need to spam the logs
		break;
	}

	mInstanceCount--; // One less instance
}

// Uses members, so make sure they are defined
// Loads the sound specified in mPath
bool Sound::load()
{
	switch(mType)
	{
	case SOUND_MUSIC:
		mMusicPointer = Mix_LoadMUS(mPath.c_str());

		if(mMusicPointer == nullptr)
		{
			Utils::CRASH_FROM_SDL("Failed to load music sound '" + mName + "' at '" + mPath + "'!");
			return false;
		}
		return true;

	case SOUND_CHUNK:
		mChunkPointer = Mix_LoadWAV(mPath.c_str()); // Hey, guess what. WAV doesn't mean it can only load .WAVE :)
		mChunkChannel = mInstanceCount; // Each Sound of type chunck will have it's own channel

		if(mChunkPointer == nullptr)
		{
			Utils::CRASH_FROM_SDL("Failed to load chunk sound '" + mName + "' at '" + mPath + "'!");
			return false;
		}
		return true;

	default:
		Utils::directly_crash("No sound type specified for sound '" + mName + "'!");
		return false;
	}

	setVolume(MIX_MAX_VOLUME); // Make sure the volume is at maximum. This is to make sure it will stay the same in the future.
}

std::string Sound::getName()
{
	return mName;
}

// Plays, don't call this to recover from a pause! Use resume() instead.
// 1 loop = plays twice
// -1 loop = forever!
// Will replace the song that is already playing
bool Sound::play(int numberOfLoops)
{
	switch(mType)
	{
	case SOUND_MUSIC:
		if(Mix_PlayMusic(mMusicPointer, numberOfLoops) < 0)
		{
			// Failed
			Utils::CRASH_FROM_SDL("Failed to play music sound '" + mName + "'!");
			return false;
		}
		return true;

	case SOUND_CHUNK:
		if(Mix_PlayChannel(mChunkChannel, mChunkPointer, numberOfLoops) < 0)
		{
			Utils::CRASH_FROM_SDL("Failed to play chunk sound '" + mName + "'!");
			return false;
		}
		return true;

	default:
		return false;
	}
}

// Pausing can't fail
void Sound::pause()
{
	switch(mType)
	{
	case SOUND_MUSIC:
		if(Mix_PausedMusic() == 1) // Already paused, is this check needed?
			break;

		Mix_PauseMusic();
		break;

	case SOUND_CHUNK:
		if(Mix_Paused(mChunkChannel) == 1) // Already paused
			break;

		Mix_Pause(mChunkChannel);
		break;

	default:
		break;
	}
}

bool Sound::isPaused()
{
	switch(mType)
	{
	case SOUND_MUSIC:
		if(Mix_PausedMusic() == 1)
			return true;
		else
			return false;

	case SOUND_CHUNK:
		if(Mix_Paused(mChunkChannel) == 1)
			return true;
		else
			return false;

	default:
		return false;
	}
}

// Can't fail
void Sound::resume()
{
	switch(mType)
	{
	case SOUND_MUSIC:
		if(Mix_PausedMusic() == 1) // Already paused, is this check needed?
			Mix_ResumeMusic();

		break;

	case SOUND_CHUNK:
		if(Mix_Paused(mChunkChannel) == 1) // Already paused
			Mix_Resume(mChunkChannel);

		break;

	default:
		break;
	}
}

void Sound::setVolume(int volume)
{
	switch(mType)
	{
	case SOUND_MUSIC:
		Mix_VolumeMusic(volume);
		break;

	case SOUND_CHUNK:
		Mix_Volume(mChunkChannel, volume);
		break;

	default:
		break;
	}
}

int Sound::getVolume()
{
	switch(mType)
	{
	case SOUND_MUSIC:
		return Mix_VolumeMusic(-1); // -1 volume will not change the volume, but will still return it

	case SOUND_CHUNK:
		return Mix_Volume(mChunkChannel, -1);

	default:
		return 0;
	}
}