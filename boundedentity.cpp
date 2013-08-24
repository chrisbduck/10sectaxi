//------------------------------------------------------------------------------
// BoundedEntity: An entity that can't move outside a specified region.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "boundedentity.h"

#include "settings.h"
#include "texturemanager.h"
#include "useful.h"

//------------------------------------------------------------------------------
// Factory
//------------------------------------------------------------------------------

Entity* createBoundedEntity(const std::vector<std::string>& lrParams)
{
	return new BoundedEntity(getFloatParam(lrParams, 0), getFloatParam(lrParams, 1));
}

EntityFactory sBoundedEntityFactory("guard", &createBoundedEntity);

//------------------------------------------------------------------------------
// BoundedEntity
//------------------------------------------------------------------------------

BoundedEntity::BoundedEntity(float lX, float lY) :
	SpriteEntity(lX, lY)
{
	setTexture(gTextureManager.load("data/guard.png"));
	float lHalfWidth = width() * 0.5f;
	float lHalfHeight = height() * 0.5f;
	mMinX = lHalfWidth;
	mMinY = lHalfHeight;
	mMaxX = Settings::getFloat("screen/width") - lHalfWidth;
	mMaxY = Settings::getFloat("screen/height") - lHalfHeight;
}

//------------------------------------------------------------------------------

void BoundedEntity::update(float lTimeDeltaSec)
{
	SpriteEntity::update(lTimeDeltaSec);
	
	if (mX > mMaxX)
	{
		mX = mMaxX - (mX - mMaxX);
		mVelX = -mVelX;
	}
	else if (mX < mMinX)
	{
		mX = mMinX + (mMinX - mX);
		mVelX = -mVelX;
	}
	
	if (mY >= mMaxY)
	{
		mY = mMaxY - (mY - mMaxY);
		mVelY = -mVelY;
	}
	else if (mY < mMinY)
	{
		mY = mMinY + (mMinY - mY);
		mVelY = -mVelY;
	}
}

//------------------------------------------------------------------------------
