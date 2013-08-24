//------------------------------------------------------------------------------
// Video: Rendering code.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef VIDEO_H
#define VIDEO_H

//------------------------------------------------------------------------------

struct SDL_Surface;

//------------------------------------------------------------------------------

class Video
{
public:
	Video();
	~Video();
	
	bool init(int lDisplayWidth, int lDisplayHeight);
	void shutDown();
	
	void clear();
	void flip();
	
	SDL_Surface* getDisplaySurface() const { return mpDisplaySurface; }
	
private:
	bool			mInitialised;
	SDL_Surface*	mpDisplaySurface;
};

extern Video gVideo;

//------------------------------------------------------------------------------

#endif // VIDEO_H
