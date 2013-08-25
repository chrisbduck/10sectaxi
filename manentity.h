//------------------------------------------------------------------------------
// ManEntity: A passenger to pick up.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef MANENTITY_H
#define MANENTITY_H

#include "spriteentity.h"

//------------------------------------------------------------------------------

class ManEntity : public CollidableEntity
{
public:
	ManEntity(float lX, float lY);
	
	virtual void triggerCollisionEvent();
};

//------------------------------------------------------------------------------

#endif // MANENTITY_H
