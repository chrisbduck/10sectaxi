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
	
	virtual const char* type() const { return "man"; }
	
	virtual void triggerCollisionEvent();
};

//------------------------------------------------------------------------------

class TargetEntity : public CollidableEntity
{
public:
	TargetEntity(float lX, float lY);
	
	virtual const char* type() const { return "target"; }
	
	virtual void triggerCollisionEvent();
	
	void setCashValue(int lValue) { mCashValue = lValue; }
	void setCashValueFromDistance(float lStartX, float lStartY);
	
private:
	int mCashValue;
};

//------------------------------------------------------------------------------

#endif // MANENTITY_H
