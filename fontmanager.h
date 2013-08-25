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
	
	enum XAlignment { kAlignLeft, kAlignXCentre, kAlignRight };
	enum YAlignment { kAlignTop, kAlignYCentre, kAlignBottom };
	
	// This text is affected by camera movement
	void renderInWorld(const char* lpText, float lX, float lY, SDL_Colour lCol);
	
	// This text is unaffected by camera movement.  You can also specify negative x and y to give positions relative to
	// the right and bottom of the screen respectively.
	void renderOnScreen(const char* lpText, float lX, float lY, SDL_Colour lCol, XAlignment lXAlign, YAlignment lYAlign = kAlignYCentre);
	
private:
	
	void renderInternal(const char* lpText, float lX, float lY, SDL_Colour lCol, XAlignment lXAlign, YAlignment lYAlign, bool lBehindCamera);
	
	bool			mInitialised;
	TTF_Font*		mpDefaultFont;
	SDL_Surface*	mpDisplaySurface;
};

extern FontManager gFontManager;

//------------------------------------------------------------------------------

#endif // FONTS_H
