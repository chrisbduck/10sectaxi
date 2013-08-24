//------------------------------------------------------------------------------
// AudioManager: Handles sounds and music.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SDL/SDL_mixer.h>
#include <string>
#include <unordered_map>

//------------------------------------------------------------------------------

class Music;
class Sound;

//------------------------------------------------------------------------------

class AudioManager
{
public:
	AudioManager();
	~AudioManager();
	
	bool init();
	void shutDown();
	
	// The load functions always return valid pointers.  If the music or sound is unavailable (or the audio manager is
	// uninitialised), it will return a valid object that does nothing when activated.
	Music* loadMusic(const std::string& lrFileName);
	Sound* loadSound(const std::string& lrFileName);
	
	bool isPlayingMusic() const;
	
private:
	bool mInitialised;
	std::unordered_map<std::string, Music*> mRegisteredMusic;
	std::unordered_map<std::string, Sound*> mRegisteredSounds;
};

extern AudioManager gAudioManager;

//------------------------------------------------------------------------------

class Music
{
	friend class AudioManager;
public:
	
	void play();
	void stop();
	static bool isPlaying();
	
private:
	// Only AudioManager should construct and destroy these objects
	Music(const std::string& lrFileName);
	~Music();
	
	Mix_Music* mpMixerMusic;
};

//------------------------------------------------------------------------------

class Sound
{
	friend class AudioManager;
public:
	
	void play();
	
private:
	// Only AudioManager should construct and destroy these objects
	Sound(const std::string& lrFileName);
	~Sound();
	
	Mix_Chunk* mpChunk;
	int mChannel;
};

//------------------------------------------------------------------------------

#endif // AUDIOMANAGER_H
