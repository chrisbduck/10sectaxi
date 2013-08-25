//------------------------------------------------------------------------------
// Camera: Determines the viewable area.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "camera.h"

#include "settings.h"
#include "useful.h"

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

bool Camera::canSee(const Entity* lpTarget) const
{
	if (lpTarget->right() <= this->left() || this->right() <= lpTarget->left())
		return false;
	if (lpTarget->bottom() <= this->top() || this->bottom() <= lpTarget->top())
		return false;
	return true;
}

//------------------------------------------------------------------------------

void Camera::updateFromPlayer(Entity *lpPlayer, float lMinX, float lMinY, float lMaxX, float lMaxY)
{
	float lMoveBorderPixels = Settings::getFloat("screen/camera_move_border");
	
	float lCameraX = this->x();
	float lCameraY = this->y();
	
	if (lpPlayer->left() < left() + lMoveBorderPixels)
		lCameraX = max(lpPlayer->left() - lMoveBorderPixels, lMinX) + halfWidth();
	else if (lpPlayer->right() > right() - lMoveBorderPixels)
		lCameraX = min(lpPlayer->right() + lMoveBorderPixels, lMaxX) - halfWidth();
	
	if (lpPlayer->top() < top() + lMoveBorderPixels)
		lCameraY = max(lpPlayer->top() - lMoveBorderPixels, lMinY) + halfHeight();
	else if (lpPlayer->bottom() > bottom() - lMoveBorderPixels)
		lCameraY = min(lpPlayer->bottom() + lMoveBorderPixels, lMaxY) - halfHeight();
	
	//if (lCameraX != this->x() || lCameraY != this->y())
		//printf("Moving camera from (%.1f, %.1f) to (%.1f, %.1f)\n", x(), y(), lCameraX, lCameraY);
	
	setPos(lCameraX, lCameraY);
}

//------------------------------------------------------------------------------
