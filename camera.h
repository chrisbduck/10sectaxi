//------------------------------------------------------------------------------
// Camera: Determines the viewable area.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef CAMERA_H
#define CAMERA_H

#include "entity.h"

//------------------------------------------------------------------------------

class Camera : public Entity
{
public:
	Camera(float lX, float lY, float lViewWidth, float lViewHeight);
	~Camera();
	
	virtual const char* type() const { return "camera"; }
	
	void set(float lX, float lY, float lViewWidth, float lViewHeight) { setPos(lX, lY); setSize(lViewWidth, lViewHeight); }
	
	float viewWidth() const			{ return width(); }
	float viewHeight() const		{ return height(); }
	float halfViewWidth() const		{ return halfWidth(); }
	float halfViewHeight() const	{ return halfHeight(); }
	
private:
};

extern Camera* gpCamera;	// there may be more than one camera, but this points to the first one

//------------------------------------------------------------------------------

#endif // CAMERA_H
