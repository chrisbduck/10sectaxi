//------------------------------------------------------------------------------
// CarEntity: A car sprite that drives around.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef CARENTITY_H
#define CARENTITY_H

#include "spriteentity.h"

//------------------------------------------------------------------------------

class CarEntity : public CollidableEntity
{
public:
	CarEntity(float lX, float lY, const std::string& lrColour);
	
	virtual const char* type() const { return "car"; }
	
	virtual void update(float lTimeDeltaSec);
	
	virtual float bounceFactor() const;
	
protected:
	
	void enforceBoundaries();
	void checkCollisions();
	
	float mSteerCtrl;			// <0 => left; >0 => right
	float mAccelCtrl;			// >0 => accelerate; <0 => brake/reverse accelerate
	float mLastAngle;			// last recorded velocity angle for the car
	bool  mReversing;			// true while in reverse
	float mSwitchDirTimeSec;	// time while holding the key to switch from forward to reverse, or vice versa
};

//------------------------------------------------------------------------------

#endif // CARENTITY_H
