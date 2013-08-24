//------------------------------------------------------------------------------
// BoundedEntity: An entity that can't move outside a specified region.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef BOUNDEDENTITY_H
#define BOUNDEDENTITY_H

#include "entitymanager.h"
#include "spriteentity.h"
#include <string>

//------------------------------------------------------------------------------

class BoundedEntity : public SpriteEntity
{
public:
	BoundedEntity(float lX, float lY);
	
	virtual const char* type() const { return "bounded"; }
	
	virtual void update(float lTimeDeltaSec);
	
protected:
	float minX() const { return mMinX; }
	float minY() const { return mMinY; }
	float maxX() const { return mMaxX; }
	float maxY() const { return mMaxY; }
	void setBounds(float lMinX, float lMinY, float lMaxX, float lMaxY)
		{ mMinX = lMinX; mMinY = lMinY; mMaxX = lMaxX; mMaxY = lMaxY; }
	
private:
	float mMinX;
	float mMinY;
	float mMaxX;
	float mMaxY;
};

//------------------------------------------------------------------------------

#endif // BOUNDEDENTITY_H
