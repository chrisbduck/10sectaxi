//------------------------------------------------------------------------------
// PlayerCarEntity: A car controlled by the player.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef PLAYERCARENTITY_H
#define PLAYERCARENTITY_H

#include "carentity.h"

//------------------------------------------------------------------------------

class PlayerCarEntity : public CarEntity
{
public:
	PlayerCarEntity(float lX, float lY);
	
	virtual const char* type() const { return "player"; }
	
	virtual void update(float lTimeDeltaSec);
};

extern PlayerCarEntity* gpPlayer;

//------------------------------------------------------------------------------

#endif // PLAYERCARENTITY_H
