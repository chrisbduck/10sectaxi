//------------------------------------------------------------------------------
// HouseEntity: An obstacle for the player's car.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef HOUSEENTITY_H
#define HOUSEENTITY_H

#include "spriteentity.h"

//------------------------------------------------------------------------------

class HouseEntity : public CollidableEntity
{
public:
	HouseEntity(float lX, float lY);
	
	virtual const char* type() const { return "house"; }
	
	virtual float bounceFactor() const;
	
	std::string getStartMessage() const;
	std::string getStartMessage2() const;
	std::string getWinMessage() const;
	std::string getLoseMessage() const;
};

//------------------------------------------------------------------------------

#endif // HOUSEENTITY_H
