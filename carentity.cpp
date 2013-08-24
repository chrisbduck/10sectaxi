//------------------------------------------------------------------------------
// CarEntity: A car sprite that drives around.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "carentity.h"

#include "settings.h"
#include "texturemanager.h"
#include "useful.h"
#include <cmath>

//------------------------------------------------------------------------------

CarEntity::CarEntity(float lX, float lY, const std::string& lrColour) :
	SpriteEntity(lX, lY),
	mSteerCtrl(0.0f),
	mAccelCtrl(0.0f)
{
	setTexture(gTextureManager.load("data/tex/" + lrColour + "-car.png"));
	setRotationStartsFromUp(true);
}

//------------------------------------------------------------------------------

void testPolarFromRect(float lX, float lY)
{
	float lMag, lAngRad;
	getPolarFromRect(lX, lY, &lMag, &lAngRad);
	printf("rect (%.3f, %.3f) = polar mag %.3f at %.3f rad\n", lX, lY, lMag, lAngRad);
}

void testRectFromPolar(float lMag, float lAngRad)
{
	float lX, lY;
	getRectFromPolar(lMag, lAngRad, &lX, &lY);
	printf("polar mag %.3f at %5.3f rad = rect (%.3f, %.3f)\n", lMag, lAngRad, lX, lY);
}

void CarEntity::update(float lTimeDeltaSec)
{
	float lVelMag, lVelAngleRad;
	getPolarFromRect(velX(), velY(), &lVelMag, &lVelAngleRad);
	printf("mag = %f; angle = %f\n", lVelMag, lVelAngleRad);
	
	if (floatApproxEquals(lVelMag, 0.0f))
		lVelAngleRad = rotation();			// use previous rotation
	
	bool lAdjustedVel = false;
	
	static const float kSteerRadsPerSecLow = Settings::getFloat("handling/steer_rads_per_sec_low");
	static const float kSteerRadsPerSecHigh = Settings::getFloat("handling/steer_rads_per_sec_high");
	static const float kLowThreshold = Settings::getFloat("handling/low_threshold");
	static const float kHighThreshold = Settings::getFloat("handling/high_threshold");
	static const float kAccelPerSecLow = Settings::getFloat("handling/accel_per_sec_low");
	static const float kAccelPerSecHigh = Settings::getFloat("handling/accel_per_sec_high");
	static const float kNoAccelSlowing = Settings::getFloat("handling/no_accel_slowing");
	static const float kBrakePerSec = Settings::getFloat("handling/brake_per_sec");
	
	float lLowHighFactor =  (lVelMag < kLowThreshold) ? 0.0f :
							(lVelMag > kHighThreshold ? 1.0f : 
							((lVelMag - kLowThreshold) / (kHighThreshold - kLowThreshold)));
	
	// Steering (only while moving)
	if (lVelMag > 0.0f && mSteerCtrl != 0.0f)
	{
		float lSteerRadsPerSec = lerp(lLowHighFactor, kSteerRadsPerSecLow, kSteerRadsPerSecHigh);
		lVelAngleRad += lTimeDeltaSec * lSteerRadsPerSec * mSteerCtrl;
		lAdjustedVel = true;
	}
	
	// Acceleration
	if (mAccelCtrl != 0.0f)
	{
		float lAccelPerSec = lerp(lLowHighFactor, kAccelPerSecLow, kAccelPerSecHigh);
		if (mAccelCtrl > 0.0f)
			lVelMag += lTimeDeltaSec * lAccelPerSec * mAccelCtrl;
		else	// <0
			lVelMag = max(lVelMag - lTimeDeltaSec * kBrakePerSec * -mAccelCtrl, 0.0f);
		lAdjustedVel = true;
	}
	// Apply drag if moving
	else if (!floatApproxEquals(lVelMag, 0.0f))
	{
		lVelMag = max(lVelMag - lTimeDeltaSec * kNoAccelSlowing, 0.0f);
		lAdjustedVel = true;
	}
	
	// Update velocity if modified
	if (lAdjustedVel)
	{
		float lVelX, lVelY;
		getRectFromPolar(lVelMag, lVelAngleRad, &lVelX, &lVelY);
		setVel(lVelX, lVelY);
		setRotation(lVelAngleRad);
	}
	
	SpriteEntity::update(lTimeDeltaSec);
}

//------------------------------------------------------------------------------
