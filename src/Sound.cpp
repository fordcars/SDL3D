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
// Also, don't expect music sounds to store their pause/volume and all if they are replaced by another
// Sound chunks are fine since each of them have their own software channel

#include "Sound.hpp"

#include "Utils.hpp"
#include "Definitions.hpp"

// Done once
// Static member initialized
int Sound::mInstanceCount = 0;
Mix_Music* Sound::mLastPlayedMusic = nullptr;

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
		mChunkChannel = mInstanceCount; // Each Sound of type chunk will have it's own channel

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

// Plays, don't call this to recover from a pause (it will restart). Use resume() instead.
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

		// Keep track of the last played music, it's useful since only one can play at a time
		mLastPlayedMusic = mMusicPointer;
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

// Check if this sound is playing
// Does NOT check if it is paused! Only checks if play() or similar was called.
bool Sound::isPlaying()
{
	switch(mType)
	{
	case SOUND_MUSIC:
		// Same music that last played and it is playing, for safety and clarity
		// mLastPlayedMusic might be null, but mMusicPointer can't
		if(mMusicPointer == mLastPlayedMusic && Mix_PlayingMusic() == 1)
				return true;

		return false; // Not the same music that last played or not playing

	case SOUND_CHUNK:
		if(Mix_Playing(mChunkChannel) == 1)
			return true;
		else
			return false;

	default:
		return false;
	}
}

// Halts the sound. You will need to call play() to play the sound again.
// Returns true on success
bool Sound::halt()
{
	if(!isPlaying()) // If it's not playing
	{
		Utils::CRASH("Sound '" + mName + "' cannot be halted since it is not playing! Please play the song before trying to halt it.");
		return false;
	}

	switch(mType)
	{
	case SOUND_MUSIC:
		if(mMusicPointer == mLastPlayedMusic)
			Mix_HaltMusic();

		break;

	case SOUND_CHUNK:
		Mix_HaltChannel(mChunkChannel);
		break;

	default:
		return false;
	}

	return true;
}

// Pausing can't fail, it will pause anything, including a halted sound.
void Sound::pause()
{
	switch(mType)
	{
	case SOUND_MUSIC:
		// Will not do anything if this is not the right music
		// mLastPlayedMusic might be null, but mMusicPointer can't
		if(mMusicPointer == mLastPlayedMusic)
			Mix_PauseMusic();

		break;

	case SOUND_CHUNK:
		Mix_Pause(mChunkChannel);
		break;

	default:
		break;
	}
}

// Does not check if the sound is playing or was halted
bool Sound::isPaused()
{
	switch(mType)
	{
	case SOUND_MUSIC:
		// We don't care if any music is playing
		// Same music that last played and is paused
		if(mMusicPointer == mLastPlayedMusic && Mix_PausedMusic() == 1)
				return true;
		
		return false; // Music not currently playing don't have a pause state, so lets say it wasn't paused

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
// Resumes from a pause
void Sound::resume()
{
	switch(mType)
	{
	case SOUND_MUSIC:
		// Don't do anything if this is not the right music
		if(mMusicPointer == mLastPlayedMusic)
			Mix_ResumeMusic();

		break;

	case SOUND_CHUNK:
		Mix_Resume(mChunkChannel);
		break;

	default:
		break;
	}
}

// Fade time in seconds
// Plays the sound (no need to call play())
// If loops is -1, it will loop forever
bool Sound::fadeIn(float fadeTime, int loops)
{
	int fadeTimeInMS = static_cast<int>(fadeTime * 1000.0f); // Convert to miliseconds

	switch(mType)
	{
	case SOUND_MUSIC:
		if(Mix_FadeInMusic(mMusicPointer, loops, fadeTimeInMS) < 0)
		{
			Utils::CRASH_FROM_SDL("Failed to fade in for music sound '" + mName + "'!");
			return false;
		}

		mLastPlayedMusic = mMusicPointer;
		return true;

	case SOUND_CHUNK:
		if(Mix_FadeInChannel(mChunkChannel, mChunkPointer, loops, fadeTimeInMS) < 0)
		{
			Utils::CRASH_FROM_SDL("Failed to fade in for chunk sound '" + mName + "'!");
			return false;
		}
		return true;

	default:
		return false;
	}
}

// In seconds
// When finished, halts the sound
bool Sound::fadeOut(float fadeTime)
{
	int fadeTimeInMS = static_cast<int>(fadeTime * 1000.0f); // Convert to miliseconds

	if(!isPlaying()) // If it's not playing
	{
		Utils::CRASH("Sound '" + mName + "' cannot be faded out since it is not playing! Please play the song before trying to fade out.");
		return false;
	}

	switch(mType)
	{
	case SOUND_MUSIC:
		if(Mix_FadeOutMusic(fadeTimeInMS) == 0) // Why SDL_mixer, WHY 0!
		{
			// Failed
			Utils::CRASH_FROM_SDL("Failed to fade out for music sound '" + mName + "'!");
			return false;
		}
		return true;

	case SOUND_CHUNK:
		if(Mix_FadeOutChannel(mChunkChannel, fadeTimeInMS) == 0) // Oh, this is why
		{
			// Failed
			Utils::CRASH_FROM_SDL("Failed to fade out for chunk sound '" + mName + "'!");
			return false;
		}
		return true;

	default:
		return false;
	}
}

// Volume is between 0 to 128 (MIX_MAX_VOLUME)
void Sound::setVolume(int volume)
{
	switch(mType)
	{
	case SOUND_MUSIC:
		if(mMusicPointer == mLastPlayedMusic) // Right music
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
		if(mMusicPointer == mLastPlayedMusic) // Right music
			return Mix_VolumeMusic(-1); // -1 volume will not change the volume, but will still return it

		return 0;

	case SOUND_CHUNK:
		return Mix_Volume(mChunkChannel, -1);

	default:
		return 0;
	}
}