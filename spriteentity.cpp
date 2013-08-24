//------------------------------------------------------------------------------
// SpriteEntity: Base class for interactive elements that have textures.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "spriteentity.h"

#include "camera.h"
#include "useful.h"
#include "video.h"

#include <SDL/SDL_surface.h>

//------------------------------------------------------------------------------

SpriteEntity::SpriteEntity(float lX, float lY) :
	Entity(lX, lY),
	mpTexture(nullptr)
{
}

//------------------------------------------------------------------------------

void SpriteEntity::setTexture(SDL_Surface* lpTexture)
{
	mpTexture = lpTexture;
	setWidth((mpTexture != nullptr) ? lpTexture->w : 0.0f);
	setHeight((mpTexture != nullptr) ? lpTexture->h : 0.0f);
}

//------------------------------------------------------------------------------

void SpriteEntity::render() const
{
	Entity::render();
	
	if (mpTexture == nullptr)
		return;
	
	float lCameraOffsetX = gpCamera->halfWidth() - gpCamera->x();
	float lCameraOffsetY = gpCamera->halfHeight() - gpCamera->y();
	SDL_Rect lRect = { int(mX + lCameraOffsetX - halfWidth()), int(mY + lCameraOffsetY - halfHeight()),
					   int(width()), int(height()) };
	
	//printf("Rect of \"%s\": (%d, %d), %d x %d\n", name().c_str(), lRect.x, lRect.y, lRect.w, lRect.h);
	
	SDL_BlitSurface(mpTexture, nullptr, gVideo.getDisplaySurface(), &lRect);
}

//------------------------------------------------------------------------------
