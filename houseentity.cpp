//------------------------------------------------------------------------------
// HouseEntity: An obstacle for the player's car.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "houseentity.h"

#include "texturemanager.h"

//------------------------------------------------------------------------------

HouseEntity::HouseEntity(float lX, float lY) :
	SpriteEntity(lX, lY)
{
	setTexture(gTextureManager.load("data/house1-128.jpg"));
}

//------------------------------------------------------------------------------
