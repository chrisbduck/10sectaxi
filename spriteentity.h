//------------------------------------------------------------------------------
// SpriteEntity: Base class for interactive elements that have textures.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef SPRITEENTITY_H
#define SPRITEENTITY_H

#include "entity.h"

//------------------------------------------------------------------------------

struct SDL_Surface;

//------------------------------------------------------------------------------

class SpriteEntity : public Entity
{
public:
	SpriteEntity(float lX, float lY);
	
	virtual const char* type() const { return "sprite"; }
	
	virtual void render() const;
	
	void setTexture(SDL_Surface* lpTexture);
	
private:
	SDL_Surface* mpTexture;
};

//------------------------------------------------------------------------------

#endif // SPRITEENTITY_H
