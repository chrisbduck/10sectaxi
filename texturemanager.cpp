//------------------------------------------------------------------------------
// TextureManager: Loads textures for use in various places.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "texturemanager.h"

#include "useful.h"

#include <SDL/SDL_image.h>
#include <SDL/SDL_surface.h>

//------------------------------------------------------------------------------

TextureManager gTextureManager;

//------------------------------------------------------------------------------

TextureManager::TextureManager()
{
}

//------------------------------------------------------------------------------

TextureManager::~TextureManager()
{
	shutDown();
}

//------------------------------------------------------------------------------

void TextureManager::init()
{
}

//------------------------------------------------------------------------------

void TextureManager::shutDown()
{
	for (auto liTexture: mTextures)
		SDL_FreeSurface(liTexture.second);
	mTextures.clear();
}

//------------------------------------------------------------------------------

SDL_Surface* TextureManager::load(const std::string &lrFileName)
{
	// Return an existing surface if available
	auto liTexture = mTextures.find(lrFileName);
	if (liTexture != mTextures.end())
		return liTexture->second;
	
	printf("Loading texture \"%s\"\n", lrFileName.c_str());
	SDL_Surface* lpSurface = IMG_Load(lrFileName.c_str());
	ASSERT2(lpSurface != nullptr, "Texture load failed.");
	mTextures[lrFileName] = lpSurface;
	return lpSurface;
}

//------------------------------------------------------------------------------
