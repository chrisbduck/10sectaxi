//------------------------------------------------------------------------------
// Video: Rendering code.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "video.h"

#include "useful.h"
#include <SDL/SDL_compat.h>
#include <SDL/SDL_surface.h>

//------------------------------------------------------------------------------

Video gVideo;

//------------------------------------------------------------------------------

Video::Video() :
	mInitialised(false),
	mpDisplaySurface(nullptr)
{
}

//------------------------------------------------------------------------------

Video::~Video()
{
	if (mInitialised)
		shutDown();
}

//------------------------------------------------------------------------------

bool Video::init(int lDisplayWidth, int lDisplayHeight)
{
	ASSERT(!mInitialised);
	
	mpDisplaySurface = SDL_SetVideoMode(lDisplayWidth, lDisplayHeight, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (mpDisplaySurface == nullptr)
	{
		printf("Failed to set SDL video mode %dx%dx32\n", lDisplayWidth, lDisplayHeight);
		return false;
	}
	
	SDL_FillRect(mpDisplaySurface, nullptr, 0xFF808080);
	SDL_Flip(mpDisplaySurface);
	
	mInitialised = true;
	return true;
}

//------------------------------------------------------------------------------

void Video::shutDown()
{
	if (!mInitialised)
		return;
	printf("Shutting down video\n");
	
	if (mpDisplaySurface != nullptr)
	{
		SDL_FreeSurface(mpDisplaySurface);
		mpDisplaySurface = nullptr;
	}
	
	mInitialised = false;
}

//------------------------------------------------------------------------------

void Video::clear()
{
	//SDL_FillRect(mpDisplaySurface, nullptr, 0xFF000000);
}

//------------------------------------------------------------------------------

void Video::flip()
{
	SDL_Flip(mpDisplaySurface);
}

//------------------------------------------------------------------------------
