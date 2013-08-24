//------------------------------------------------------------------------------
// Camera: Determines the viewable area.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "camera.h"

//------------------------------------------------------------------------------

Camera* gpCamera = nullptr;

//------------------------------------------------------------------------------

Camera::Camera(float lX, float lY, float lViewWidth, float lViewHeight) :
	Entity(lX, lY)
{
	setSize(lViewWidth, lViewHeight);
	if (gpCamera == nullptr)
		gpCamera = this;
}

//------------------------------------------------------------------------------

Camera::~Camera()
{
	if (gpCamera == this)
		gpCamera = nullptr;
}

//------------------------------------------------------------------------------
