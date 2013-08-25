//------------------------------------------------------------------------------
// PlayerCarEntity: A car controlled by the player.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "playercarentity.h"

#include "useful.h"
#include <SDL/SDL_keyboard.h>

//------------------------------------------------------------------------------

PlayerCarEntity* gpPlayer = nullptr;

//------------------------------------------------------------------------------

PlayerCarEntity::PlayerCarEntity(float lX, float lY) :
	CarEntity(lX, lY, "yellow")
{
	ASSERT(gpPlayer == nullptr);
	gpPlayer = this;
}

//------------------------------------------------------------------------------

void PlayerCarEntity::update(float lTimeDeltaSec)
{
	// Process player input
	int lNumKeys;
	uint8_t* lpKeysHeld = SDL_GetKeyboardState(&lNumKeys);
	
	/*bool lAnyHeld = false;
	for (int lKey = 0; lKey < lNumKeys; ++lKey)
		if (lpKeysHeld[lKey])
		{
			if (!lAnyHeld)
				printf("Keys held: ");
			printf("%d ", lKey);
			lAnyHeld = true;
		}
	if (lAnyHeld)
		printf("\n");*/
	
	// Brake/accelerate
	if (lpKeysHeld[SDLK_KP_2] || lpKeysHeld[SDLK_DOWN])
		mAccelCtrl = -1.0f;
	else if (lpKeysHeld[SDLK_KP_8] || lpKeysHeld[SDLK_UP])
		mAccelCtrl = 1.0f;
	else
		mAccelCtrl = 0.0f;
	// Left/right
	bool lLeft  = lpKeysHeld[SDLK_KP_4] || lpKeysHeld[SDLK_LEFT];
	bool lRight = lpKeysHeld[SDLK_KP_6] || lpKeysHeld[SDLK_RIGHT];
	if (lLeft ^ lRight)
		mSteerCtrl = lLeft ? -1.0f : +1.0f;
	else
		mSteerCtrl = 0.0f;
	
	//if (mAccelCtrl != 0.0f || mSteerCtrl != 0.0f)
		//printf("Controls: accel %.1f, steer %.1f\n", mAccelCtrl, mSteerCtrl);
	
	CarEntity::update(lTimeDeltaSec);
}

//------------------------------------------------------------------------------
