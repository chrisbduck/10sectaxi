//------------------------------------------------------------------------------
// RectEntity: Rendered rectangles.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#if 0
#ifndef RECTENTITY_H
#define RECTENTITY_H

#include "entity.h"

//------------------------------------------------------------------------------

class RectEntity : public Entity
{
public:
	RectEntity(float lX, float lY);
	
	virtual void render() const;
};

//------------------------------------------------------------------------------

#endif // RECTENTITY_H
#endif	// 0
