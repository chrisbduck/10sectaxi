//------------------------------------------------------------------------------
// fonts: Handles font management and rendering.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "fontmanager.h"

#include "settings.h"
#include "useful.h"

//------------------------------------------------------------------------------

FontManager gFontManager;

//------------------------------------------------------------------------------

FontManager::FontManager() :
	mInitialised(false),
	mpDefaultFont(nullptr)
{
}

//------------------------------------------------------------------------------

FontManager::~FontManager()
{
	if (mInitialised)
		shutDown();
}

//------------------------------------------------------------------------------

bool FontManager::init(SDL_Surface* lpDisplaySurface)
{
	ASSERT(!mInitialised);
	
	mpDisplaySurface = lpDisplaySurface;
	
	if (TTF_Init() < 0)
	{
		printf("TTF initialisation failed\n");
		return false;
	}
	
	std::string lDefaultFontFace = Settings::getString("default_font/face");
	int lDefaultPointSize = Settings::getInt("default_font/point_size");
	printf("Opening default font \"%s\" with size %d\n", lDefaultFontFace.c_str(), lDefaultPointSize);
	if (lDefaultFontFace.empty() || lDefaultPointSize == 0)
	{
		printf("Invalid default font\n");
		return false;
	}
	mpDefaultFont = TTF_OpenFont(lDefaultFontFace.c_str(), lDefaultPointSize);
	if (mpDefaultFont == nullptr)
	{
		printf("Error loading default font\n");
		return false;
	}
	
	mInitialised = true;
	return true;
}

//------------------------------------------------------------------------------

void FontManager::shutDown()
{
	ASSERT(mInitialised);
	
	TTF_CloseFont(mpDefaultFont);
	mpDefaultFont = nullptr;
	
	mInitialised = false;
}

//------------------------------------------------------------------------------

void FontManager::render(const char* lpText, int lX, int lY, SDL_Colour lCol)
{
	ASSERT(mInitialised);
	
	SDL_Surface* lpRenderedFontSurface = TTF_RenderText_Blended(mpDefaultFont, lpText, lCol);
	
	SDL_Rect lTargetRect = { lX, lY, lpRenderedFontSurface->w, lpRenderedFontSurface->h };
	
	SDL_BlitSurface(lpRenderedFontSurface, nullptr, mpDisplaySurface, &lTargetRect);
	SDL_FreeSurface(lpRenderedFontSurface);
}

//------------------------------------------------------------------------------
