//------------------------------------------------------------------------------
// ManEntity: A passenger to pick up.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "manentity.h"

#include "app.h"
#include "texturemanager.h"
#include <emscripten/emscripten.h>
#include <string>
#include <sstream>

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
	gApplication.addCash(5 + int(emscripten_random() * 6.0f));
	gApplication.setStatusMessage("Please take me somewhere!");
	kill();
}

//------------------------------------------------------------------------------
