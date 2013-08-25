//------------------------------------------------------------------------------
// CarEntity: A car sprite that drives around.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "carentity.h"

#include "app.h"
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

void CarEntity::update(float lTimeDeltaSec)
{
	float lVelMag, lVelAngleRad;
	getPolarFromRect(velX(), velY(), &lVelMag, &lVelAngleRad);
	//printf("mag = %f; angle = %f\n", lVelMag, lVelAngleRad);
	
	float lFacingAngleRad = rotationRad();
	float lFacingXNorm, lFacingYNorm;
	getRectFromPolar(1.0f, lFacingAngleRad, &lFacingXNorm, &lFacingYNorm);
	float lFacingSpeed = velX() * lFacingXNorm + velY() * lFacingYNorm;
	
	float lTangXNorm, lTangYNorm;
	getRectFromPolar(1.0f, lFacingAngleRad + M_PI_OVER_2, &lTangXNorm, &lTangYNorm);
	float lTangSpeed = velX() * lTangXNorm + velY() * lTangYNorm;
	
	if (floatApproxEquals(lVelMag, 0.0f))
		lVelAngleRad = lFacingAngleRad;			// use previous rotation
	
	static const float kSteerRadsPerSecLow		= Settings::getFloat("handling/steer_rads_per_sec_low");
	static const float kSteerRadsPerSecHigh		= Settings::getFloat("handling/steer_rads_per_sec_high");
	static const float kLowThreshold			= Settings::getFloat("handling/low_threshold");
	static const float kHighThreshold			= Settings::getFloat("handling/high_threshold");
	static const float kAccelPerSecLow			= Settings::getFloat("handling/accel_per_sec_low");
	static const float kAccelPerSecHigh			= Settings::getFloat("handling/accel_per_sec_high");
	static const float kNoAccelSlowing			= Settings::getFloat("handling/no_accel_slowing");
	static const float kBrakePerSec				= Settings::getFloat("handling/brake_per_sec");
	static const float kGrip					= Settings::getFloat("handling/grip");
	static const float kGripFactorWhenBraking	= Settings::getFloat("handling/grip_factor_when_braking");
	
	float lLowHighFactor =  (lVelMag < kLowThreshold) ? 0.0f :
							(lVelMag > kHighThreshold ? 1.0f : 
							((lVelMag - kLowThreshold) / (kHighThreshold - kLowThreshold)));
	
	// Steering (only while moving)
	if (lVelMag > 0.0f && mSteerCtrl != 0.0f)
	{
		// Below the low threshold, steering drops to zero (can't turn when stationary)
		float lSteerRadsPerSec = (lVelMag < kLowThreshold)
								 ? lerp(lVelMag/ kLowThreshold, 0.0f, kSteerRadsPerSecLow)
								 : lerp(lLowHighFactor, kSteerRadsPerSecLow, kSteerRadsPerSecHigh);
		
		lFacingAngleRad += lTimeDeltaSec * lSteerRadsPerSec * mSteerCtrl;
	}
	
	// Acceleration
	if (mAccelCtrl != 0.0f)
	{
		float lAccelPerSec = lerp(lLowHighFactor, kAccelPerSecLow, kAccelPerSecHigh);
		if (mAccelCtrl > 0.0f)
		{
			float lAccelMag = lTimeDeltaSec * lAccelPerSec * mAccelCtrl;
			lFacingSpeed += lAccelMag;
		}
		else	// <0
		{
			float lBrakeMag = lTimeDeltaSec * kBrakePerSec * -mAccelCtrl;
			lFacingSpeed = max(lFacingSpeed - lBrakeMag, 0.0f);
		}
	}
	// Apply in-line drag if moving
	else if (!floatApproxEquals(lFacingSpeed, 0.0f))
		lFacingSpeed = max(lFacingSpeed - lTimeDeltaSec * kNoAccelSlowing, 0.0f);
	
	// Apply tangential drag
	if (!floatApproxEquals(lTangSpeed, 0.0f))
	{
		float lGripFactor = kGrip;
		if (mAccelCtrl < 0.0f)
			lGripFactor *= kGripFactorWhenBraking;
		lTangSpeed = max(lTangSpeed - lTimeDeltaSec * lGripFactor * kBrakePerSec, 0.0f);
	}
	
	// Reconstruct velocity
	mVelX = lFacingXNorm * lFacingSpeed + lTangXNorm * lTangSpeed;
	mVelY = lFacingYNorm * lFacingSpeed + lTangYNorm * lTangSpeed;
	
	setRotationRad(lFacingAngleRad);
	
	SpriteEntity::update(lTimeDeltaSec);
	
	// Enforce boundaries
	Application& lrApp = gApplication;
	if (left() < lrApp.areaLeft())
	{
		setX(lrApp.areaLeft() + halfWidth());
		if (velX() < 0.0f)
			setVelX(0.0f);
	}
	else if (right() > lrApp.areaRight())
	{
		setX(lrApp.areaRight() - halfWidth());
		if (velX() > 0.0f)
			setVelX(0.0f);
	}
	if (top() < lrApp.areaTop())
	{
		setY(lrApp.areaTop() + halfHeight());
		if (velY() < 0.0f)
			setVelY(0.0f);
	}
	else if (bottom() > lrApp.areaBottom())
	{
		setY(lrApp.areaBottom() - halfHeight());
		if (velY() > 0.0f)
			setVelY(0.0f);
	}
}

//------------------------------------------------------------------------------
