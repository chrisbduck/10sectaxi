//------------------------------------------------------------------------------
// FontManager: Handles font management and rendering.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef FONTS_H
#define FONTS_H

#include <SDL/SDL_ttf.h>

//------------------------------------------------------------------------------

struct SDL_Surface;

//------------------------------------------------------------------------------

class FontManager
{
public:
	FontManager();
	~FontManager();
	
	bool init(SDL_Surface* lpDisplaySurface);
	void shutDown();
	
	void render(const char* lpText, int lX, int lY, SDL_Colour lCol);
	
private:
	bool			mInitialised;
	TTF_Font*		mpDefaultFont;
	SDL_Surface*	mpDisplaySurface;
};

extern FontManager gFontManager;

//------------------------------------------------------------------------------

#endif // FONTS_H
