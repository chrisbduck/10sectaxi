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
// TextureManager
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
		delete liTexture.second;
	mTextures.clear();
}

//------------------------------------------------------------------------------

Texture* TextureManager::load(const std::string &lrFileName)
{
	// Return an existing surface if available
	auto liTexture = mTextures.find(lrFileName);
	if (liTexture != mTextures.end())
		return liTexture->second;
	
	printf("Loading texture \"%s\"\n", lrFileName.c_str());
	SDL_Surface* lpSurface = IMG_Load(lrFileName.c_str());
	ASSERT2(lpSurface != nullptr, "Texture load failed.");
	Texture* lpTexture = new Texture(lpSurface);
	mTextures[lrFileName] = lpTexture;
	return lpTexture;
}

//------------------------------------------------------------------------------
// Texture
//------------------------------------------------------------------------------

Texture::Texture(SDL_Surface* lpSurface) :
	mpSurface(lpSurface),
	mTexID(0)
{
	if (mpSurface != nullptr)
	{
		glGenTextures(1, &mTexID);
		glBindTexture(GL_TEXTURE_2D, mTexID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mpSurface->w, mpSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, mpSurface->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

//------------------------------------------------------------------------------

Texture::~Texture()
{
	if (mpSurface != nullptr)
	{
		SDL_FreeSurface(mpSurface);
		mpSurface = nullptr;
	}
}

//------------------------------------------------------------------------------

void Texture::activate(GLenum lTextureStage)
{
	glActiveTexture(lTextureStage);
	glBindTexture(GL_TEXTURE_2D, mTexID);	// even if it's 0
}

//------------------------------------------------------------------------------

int Texture::width() const
{
	return mpSurface != nullptr ? mpSurface->w : 0;
}

//------------------------------------------------------------------------------

int Texture::height() const
{
	return mpSurface != nullptr ? mpSurface->h : 0;
}

//------------------------------------------------------------------------------
