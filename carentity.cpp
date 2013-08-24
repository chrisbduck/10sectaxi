//------------------------------------------------------------------------------
// CarEntity: A car sprite that drives around.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "carentity.h"

#include "texturemanager.h"

//------------------------------------------------------------------------------

CarEntity::CarEntity(float lX, float lY, const std::string& lrColour) :
	SpriteEntity(lX, lY)
{
	setTexture(gTextureManager.load("data/tex/" + lrColour + "-car.png"));
}

//------------------------------------------------------------------------------
