//------------------------------------------------------------------------------
// HouseEntity: An obstacle for the player's car.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "houseentity.h"

#include "settings.h"
#include "texturemanager.h"

//------------------------------------------------------------------------------

HouseEntity::HouseEntity(float lX, float lY) :
	CollidableEntity(lX, lY)
{
}

//------------------------------------------------------------------------------

float HouseEntity::bounceFactor() const
{
	static float kFactor = Settings::getFloat("collision/house_bounce_factor");
	return kFactor;
}

//------------------------------------------------------------------------------
