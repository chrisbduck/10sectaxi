//------------------------------------------------------------------------------
// CarEntity: A car sprite that drives around.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef CARENTITY_H
#define CARENTITY_H

#include "spriteentity.h"

//------------------------------------------------------------------------------

class CarEntity : public SpriteEntity
{
public:
	CarEntity(float lX, float lY, const std::string& lrColour);
	
	virtual const char* type() const { return "car"; }
};

//------------------------------------------------------------------------------

#endif // CARENTITY_H
