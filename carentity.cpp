//------------------------------------------------------------------------------
// CarEntity: A car sprite that drives around.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "carentity.h"

#include "app.h"
#include "entitymanager.h"
#include "settings.h"
#include "texturemanager.h"
#include "useful.h"
#include <cmath>

//------------------------------------------------------------------------------

CarEntity::CarEntity(float lX, float lY, const std::string& lrColour) :
	CollidableEntity(lX, lY),
	mSteerCtrl(0.0f),
	mAccelCtrl(0.0f),
	mLastAngle(0.0f),
	mReversing(false),
	mSwitchDirTimeSec(0.0f)
{
	setTexture(gTextureManager.load("data/tex/" + lrColour + "-car.png"));
	//setRotationStartsFromUp(true);
	setUsesCircleCollisions(true);
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
	
	float lEffectiveAccelCtrl = max( mAccelCtrl, 0.0f);		// [0, 1]
	float lEffectiveBrakeCtrl = max(-mAccelCtrl, 0.0f);		// [0, 1] - so positive when braking
	
	bool lMoving = true;
	bool lSwitching = false;
	if (floatApproxEquals(lVelMag, 0.0f))
	{
		lVelAngleRad = lFacingAngleRad;			// use previous rotation
		lMoving = false;
		if ((!mReversing && mAccelCtrl < 0.0f) || (mReversing && mAccelCtrl > 0.0f))
		{
			mSwitchDirTimeSec += lTimeDeltaSec;
			static const float kAutoreverseHoldTimeSec = Settings::getFloat("handling/autoreverse_hold_time_sec");
			if (mSwitchDirTimeSec >= kAutoreverseHoldTimeSec)
			{
				mReversing = !mReversing;
				mSwitchDirTimeSec = 0.0f;
			}
			lEffectiveAccelCtrl = 0.0f;
			lEffectiveBrakeCtrl = 0.0f;
			lSwitching = true;
		}
	}
	if (!lSwitching)
		mSwitchDirTimeSec = 0.0f;
	
	if (mReversing)
		std::swap(lEffectiveAccelCtrl, lEffectiveBrakeCtrl);
	
	//printf("Accel %.1f, brake %.1f, %s; current speed %.1f facing / %.1f tang\n",
	//	   lEffectiveAccelCtrl, lEffectiveBrakeCtrl, mReversing ? "rev" : "fwd", lFacingSpeed, lTangSpeed);
	
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
	if (lEffectiveAccelCtrl > 0.0f)
	{
		float lAccelPerSec = lerp(lLowHighFactor, kAccelPerSecLow, kAccelPerSecHigh);
		float lAccelMag = lTimeDeltaSec * lAccelPerSec * lEffectiveAccelCtrl;
		if (mReversing)
			lAccelMag = -lAccelMag;
		lFacingSpeed += lAccelMag;
	}
	else if (lEffectiveBrakeCtrl > 0.0f)
	{
		float lSign = sign(lFacingSpeed);
		float lBrakeMag = lTimeDeltaSec * kBrakePerSec * lEffectiveBrakeCtrl;
		lFacingSpeed = max(fabsf(lFacingSpeed) - lBrakeMag, 0.0f) * lSign;
	}
	// Apply in-line drag if moving
	else if (!floatApproxEquals(lFacingSpeed, 0.0f))
	{
		float lSign = sign(lFacingSpeed);
		lFacingSpeed = max(fabsf(lFacingSpeed) - lTimeDeltaSec * kNoAccelSlowing, 0.0f) * lSign;
	}
	
	// Apply tangential drag
	if (!floatApproxEquals(lTangSpeed, 0.0f))
	{
		float lGripFactor = kGrip;
		if (lEffectiveBrakeCtrl < 0.0f)
			lGripFactor *= kGripFactorWhenBraking;
		lTangSpeed = max(lTangSpeed - lTimeDeltaSec * lGripFactor * kBrakePerSec, 0.0f);
	}
	
	// Reconstruct velocity
	mVelX = lFacingXNorm * lFacingSpeed + lTangXNorm * lTangSpeed;
	mVelY = lFacingYNorm * lFacingSpeed + lTangYNorm * lTangSpeed;
	
	setRotationRad(lFacingAngleRad);
	
	if (lMoving)
		checkCollisions();
	
	CollidableEntity::update(lTimeDeltaSec);
	
	enforceBoundaries();
}

//------------------------------------------------------------------------------

void CarEntity::enforceBoundaries()
{
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

void CarEntity::checkCollisions()
{
	for (Entity* lpEntity: gEntityManager.allEntities())
	{
		if (lpEntity == this)
			continue;
		
		// Only check collidables
		CollidableEntity* lpCollidable = dynamic_cast<CollidableEntity*>(lpEntity);
		if (lpCollidable != nullptr)
			checkCollisionWith(lpCollidable);
	}
}

//------------------------------------------------------------------------------

float CarEntity::bounceFactor() const
{
	static float kFactor = Settings::getFloat("collision/car_bounce_factor");
	return kFactor;
}

//------------------------------------------------------------------------------
