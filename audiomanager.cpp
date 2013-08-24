//------------------------------------------------------------------------------
// AudioManager: Handles sounds and music.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "audiomanager.h"

#include "useful.h"

//------------------------------------------------------------------------------
// AudioManager
//------------------------------------------------------------------------------

AudioManager gAudioManager;

//------------------------------------------------------------------------------

AudioManager::AudioManager() :
	mInitialised(false)
{
}

//------------------------------------------------------------------------------

AudioManager::~AudioManager()
{
	if (mInitialised)
		shutDown();
}

//------------------------------------------------------------------------------

bool AudioManager::init()
{
	ASSERT(!mInitialised);
	
	if (Mix_Init(MIX_INIT_OGG) == 0)
	{
		printf("Failed to initialise mixer\n");
		return false;
	}
	// Default frequency is 22kHz, but I can't hear a difference to 11 or 44.  Emscripten's version may be ignoring it.
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0)
	{
		printf("Failed to open mixer audio\n");
		return false;
	}
	
	mInitialised = true;
	return true;
}

//------------------------------------------------------------------------------

void AudioManager::shutDown()
{
	ASSERT(mInitialised);
	
	for (auto liSound: mRegisteredSounds)
		delete liSound.second;
	mRegisteredSounds.clear();
	
	for (auto liMusic: mRegisteredMusic)
		delete liMusic.second;
	mRegisteredMusic.clear();
	
	Mix_CloseAudio();
	Mix_Quit();
	
	mInitialised = false;
}

//------------------------------------------------------------------------------

Music* AudioManager::loadMusic(const std::string &lrFileName)
{
	// Pass in an empty string when not initialised, so the music to play is valid but non-functional
	std::string lLoadFileName = mInitialised ? lrFileName : std::string();
	
	// Check for any already-loaded music
	auto liMusicPos = mRegisteredMusic.find(lLoadFileName);
	if (liMusicPos != mRegisteredMusic.end())
		return liMusicPos->second;
	
	// Load new music
	Music* lpMusic = new Music(lLoadFileName);
	mRegisteredMusic[lLoadFileName] = lpMusic;
	return lpMusic;
}

//------------------------------------------------------------------------------

Sound* AudioManager::loadSound(const std::string &lrFileName)
{
	// Pass in an empty string when not initialised, so the sound to play is valid but non-functional
	std::string lLoadFileName = mInitialised ? lrFileName : std::string();
	
	// Check for any already-loaded sound
	auto liSoundPos = mRegisteredSounds.find(lLoadFileName);
	if (liSoundPos != mRegisteredSounds.end())
		return liSoundPos->second;
	
	// Load new sound
	Sound* lpSound = new Sound(lLoadFileName);
	mRegisteredSounds[lLoadFileName] = lpSound;
	return lpSound;
}

//------------------------------------------------------------------------------

bool AudioManager::isPlayingMusic() const
{
	return Music::isPlaying();
}

//------------------------------------------------------------------------------
// Music
//------------------------------------------------------------------------------

Music::Music(const std::string &lrFileName) :
	mpMixerMusic(nullptr)
{
	if (!lrFileName.empty())	// ignore empty file names; use as empty music
	{
		mpMixerMusic = Mix_LoadMUS(lrFileName.c_str());
		if (mpMixerMusic == nullptr)
			printf("Error loading music: \"%s\"\n", lrFileName.c_str());
	}
}

//------------------------------------------------------------------------------

Music::~Music()
{
	if (mpMixerMusic != nullptr)
	{
		Mix_FreeMusic(mpMixerMusic);
		mpMixerMusic = nullptr;
	}
}

//------------------------------------------------------------------------------

void Music::play()
{
	if (mpMixerMusic != nullptr)
		Mix_PlayMusic(mpMixerMusic, -1);
}

//------------------------------------------------------------------------------

void Music::stop()
{
	// No point in fading; it doesn't work on Emscripten at present.  We also for now just stop all music rather than
	// checking that this is valid and the one currently playing.
	Mix_HaltMusic();
}

//------------------------------------------------------------------------------

bool Music::isPlaying()
{
	// This, for now, just returns if we're playing any music.
	return Mix_PlayingMusic() != 0;
}

//------------------------------------------------------------------------------
// Sound
//------------------------------------------------------------------------------

Sound::Sound(const std::string &lrFileName) :
	mpChunk(nullptr),
	mChannel(-1)
{
	if (!lrFileName.empty())	// ignore empty file names; use as empty sound
	{
		mpChunk = Mix_LoadWAV(lrFileName.c_str());	// works fine with non-WAVs as well
		if (mpChunk == nullptr)
			printf("Error loading sound: \"%s\"\n", lrFileName.c_str());
	}
}

//------------------------------------------------------------------------------

Sound::~Sound()
{
	if (mpChunk != nullptr)
	{
		Mix_FreeChunk(mpChunk);
		mpChunk = nullptr;
	}
}

//------------------------------------------------------------------------------

void Sound::play()
{
	if (mpChunk == nullptr)
		return;
	
	const int kFirstFreeChannel = -1;
	mChannel = Mix_PlayChannel(kFirstFreeChannel, mpChunk, 0);
}

//------------------------------------------------------------------------------
