//------------------------------------------------------------------------------
// fonts: Handles font management and rendering.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "fontmanager.h"

#include "settings.h"
#include "spriteentity.h"
#include "texturemanager.h"
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

void FontManager::renderOnScreen(const char *lpText, float lX, float lY, SDL_Color lCol, XAlignment lXAlign, YAlignment lYAlign)
{
	if (lX < 0.0f)
		lX += Settings::getFloat("screen/width");
	if (lY < 0.0f)
		lY += Settings::getFloat("screen/height");
	
	const bool kBehindCamera = true;
	renderInternal(lpText, lX, lY, lCol, lXAlign, lYAlign, kBehindCamera);
}

//------------------------------------------------------------------------------

void FontManager::renderInWorld(const char *lpText, float lX, float lY, SDL_Color lCol)
{
	const bool kNotBehindCamera = false;
	renderInternal(lpText, lX, lY, lCol, kAlignXCentre, kAlignYCentre, kNotBehindCamera);
}

//------------------------------------------------------------------------------

void FontManager::renderInternal(const char* lpText, float lX, float lY, SDL_Colour lCol, XAlignment lXAlign, YAlignment lYAlign, bool lBehindCamera)
{
	ASSERT(mInitialised);
	
	// Create the rendered surface and put it in a temporary texture
	SDL_Surface* lpRenderedFontSurface = TTF_RenderText_Blended(mpDefaultFont, lpText, lCol);
	// Note that we lock the surface but don't unlock it.  The SDL unlock call hits an assertion failure, and we only
	// need the pixels temporarily anyway.  This may be a tiny bit unreliable, but let's hope it's fine.
	SDL_LockSurface(lpRenderedFontSurface);
	Texture lTex(lpRenderedFontSurface, Texture::FilteringType::kNearest);
	
	// Adjust for alignment
	if (lXAlign == kAlignRight)
		lX -= float(lpRenderedFontSurface->w) * 0.5f;
	else if (lXAlign == kAlignLeft)
		lX += float(lpRenderedFontSurface->w) * 0.5f;
	if (lYAlign == kAlignBottom)
		lY -= float(lpRenderedFontSurface->h) * 0.5f;
	else if (lYAlign == kAlignTop)
		lY += float(lpRenderedFontSurface->h) * 0.5f;
	
	// Create a temporary sprite and render it
	SpriteEntity lTempSprite(lX, lY);
	lTempSprite.setTexture(&lTex);
	lTempSprite.setBehindCamera(lBehindCamera);
	lTempSprite.render();
}

//------------------------------------------------------------------------------
