//------------------------------------------------------------------------------
// Entity: Base class for interactive elements.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "entity.h"

#include "useful.h"
#include <SDL/SDL_surface.h>
#include <cmath>

//------------------------------------------------------------------------------
// Entity
//------------------------------------------------------------------------------

Entity::Entity(float lX, float lY) :
	mX(lX),
	mY(lY),
	mVelX(0.0f),
	mVelY(0.0f),
	mWidth(0.0f),
	mHeight(0.0f)
{
}

//------------------------------------------------------------------------------

void Entity::update(float lTimeDeltaSec)
{
	mX += mVelX * lTimeDeltaSec;
	mY += mVelY * lTimeDeltaSec;
}

//------------------------------------------------------------------------------

void Entity::getSpeedAndDir(float* lpSpeedOut, float* lpDirRadOut) const
{
	getPolarFromRect(mVelX, mVelY, lpSpeedOut, lpDirRadOut);
}

//------------------------------------------------------------------------------

float Entity::speed() const
{
	return sqrtf(mVelX * mVelX + mVelY * mVelY);
}

//------------------------------------------------------------------------------
