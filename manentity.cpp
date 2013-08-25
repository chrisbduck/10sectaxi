//------------------------------------------------------------------------------
// ManEntity: A passenger to pick up.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "manentity.h"

#include "app.h"
#include "entitymanager.h"
#include "houseentity.h"
#include "settings.h"
#include "texturemanager.h"
#include <emscripten/emscripten.h>
#include <string>
#include <sstream>

//------------------------------------------------------------------------------
// ManEntity
//------------------------------------------------------------------------------

ManEntity::ManEntity(float lX, float lY) :
	CollidableEntity(lX, lY)
{
	int lTexIndex = 1 + int(emscripten_random() * 5.0f);
	std::string lTexName = (std::ostringstream() << "data/tex/man-" << lTexIndex << ".png").str();
	setTexture(gTextureManager.load(lTexName));
	
	setCollisionTriggersEvent(true);
}

//------------------------------------------------------------------------------

void ManEntity::triggerCollisionEvent()
{
	// Don't pick up a passenger if we already have one
	if (gApplication.havePassenger())
		return;
	
	HouseEntity* lpHouse = gApplication.pickRandomHouse();
	TargetEntity* lpTarget = new TargetEntity(lpHouse->x(), 0.0f);
	float lTargetY = lpHouse->y() + lpHouse->halfHeight() + lpTarget->halfHeight();
	lpTarget->setY(lTargetY);
	lpTarget->setCashValueFromDistance(x(), y());
	gEntityManager.registerEntity(lpTarget);
	
	gApplication.setStatusMessage(lpHouse->getStartMessage(), lpHouse->getStartMessage2());
	gApplication.setCurrentTarget(lpTarget);
	gApplication.startCountdown();
	
	kill();
}

//------------------------------------------------------------------------------
// TargetEntity
//------------------------------------------------------------------------------

TargetEntity::TargetEntity(float lX, float lY) :
	CollidableEntity(lX, lY),
	mCashValue(1)
{
	setTexture(gTextureManager.load("data/tex/x.png"));
	setCollisionTriggersEvent(true);
}

//------------------------------------------------------------------------------

void TargetEntity::setCashValueFromDistance(float lStartX, float lStartY)
{
	static const float kMinThreshold = Settings::getFloat("target/min_threshold");
	static const float kMaxThreshold = Settings::getFloat("target/max_threshold");
	static const float kMinCash = Settings::getFloat("target/min_cash");
	static const float kMaxCash = Settings::getFloat("target/max_cash");
	
	// Calculate the Manhattan distance and use that for a cash value
	float lDist = fabsf(x() - lStartX) + fabsf(y() - lStartY);
	
	float lValue = lerp((lDist - kMinThreshold) / (kMaxThreshold - kMinThreshold), kMinCash, kMaxCash);
	setCashValue(int(lValue));
}

//------------------------------------------------------------------------------

void TargetEntity::triggerCollisionEvent()
{
	gApplication.winPassenger(mCashValue);
	kill();
}

//------------------------------------------------------------------------------
