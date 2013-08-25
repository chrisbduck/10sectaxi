//------------------------------------------------------------------------------
// HouseEntity: An obstacle for the player's car.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef HOUSEENTITY_H
#define HOUSEENTITY_H

#include "spriteentity.h"

//------------------------------------------------------------------------------

class HouseEntity : public SpriteEntity
{
public:
	HouseEntity(float lX, float lY);
	
	virtual const char* type() const { return "house"; }
};

//------------------------------------------------------------------------------

#endif // HOUSEENTITY_H
