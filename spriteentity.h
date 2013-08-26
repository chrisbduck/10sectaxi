//------------------------------------------------------------------------------
// SpriteEntity: Base class for interactive elements that have textures.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef SPRITEENTITY_H
#define SPRITEENTITY_H

#include "entity.h"

#include "useful.h"
#include <GLES2/gl2.h>

//------------------------------------------------------------------------------

class Texture;

//------------------------------------------------------------------------------

class SpriteEntity : public Entity
{
public:
	SpriteEntity(float lX, float lY);
	
	virtual const char* type() const { return "sprite"; }
	
	virtual void render() const;
	
	void setTexture(Texture* lpTexture);
	
	uint32_t colour() const									{ return u32ColFromFloats(mColour); }
	void setColour(uint32_t lColour)						{ getFloatColsFromU32(lColour, mColour); }
	void setColour(float lR, float lG, float lB)			{ setColour(1.0f, lR, lG, lB); }
	void setColour(float lA, float lR, float lG, float lB)	{ mColour[0] = lA; mColour[1] = lR; mColour[2] = lG; mColour[3] = lB; }
	
	float rotationRad() const								{ return mRotationRad; }
	void setRotationRad(float lRotation)					{ mRotationRad = lRotation; }
	float fixedRotationRad() const;
	
	void setBlendEnabled(bool lEnabled)						{ mBlendEnabled = lEnabled; }
	void setBehindCamera(bool lBehind)						{ mBehindCamera = lBehind; }
	
protected:
	void setRotationStartsFromUp(bool lEnabled)				{ mRotationStartsFromUp = lEnabled; }	// for car sprite, etc
	
private:
	
	static void staticInit();
	
	
	Texture* mpTexture;
	float mRotationRad;
	float mColour[4];			// ARGB
	bool mRotationStartsFromUp;
	bool mBlendEnabled;
	bool mBehindCamera;			// the sprite's position is only affected by camera movement if this is false
	
	// All sprites are rendered using a common set of vertex positions and UVs, and a set shader programme
	static bool msStaticInitDone;
	static GLfloat msVertPosData[];
	static GLfloat msVertUVData[];
	static GLuint msVertPosBufferID;
	static GLuint msVertUVBufferID;
	static GLuint msShaderProg;
	static GLuint msPosAttributeID, msUVAttributeID;
	static GLuint msMatUniformID, msColUniformID, msTexUniformID;
	static float msScreenScaleX, msScreenScaleY;
};

//------------------------------------------------------------------------------

class CollidableEntity : public SpriteEntity
{
public:
	CollidableEntity(float lX, float lY);
	
	virtual const char* type() const { return "collidable"; }
	
	virtual void update(float lTimeDeltaSec);
	
	bool checkCollisionWith(CollidableEntity* lpOther);
	bool usesCircleCollisions() const { return mUsesCircleCollisions; }
	virtual float bounceFactor() const;
	
	bool collisionTriggersEvent() const { return mCollisionTriggersEvent; }
	virtual void triggerCollisionEvent() {}
	
protected:
	void setUsesCircleCollisions(bool lEnabled)				{ mUsesCircleCollisions = lEnabled; }
	void setCollisionTriggersEvent(bool lTriggers)			{ mCollisionTriggersEvent = lTriggers; }
	
	void getRotatedBoundingBox(float* lpLeftOut, float* lpTopOut, float* lpRightOut, float* lpBottomOut) const;
	
private:
	bool mUsesCircleCollisions;
	bool mCollisionTriggersEvent;
	float mSoundDelaySec;
};

//------------------------------------------------------------------------------

#endif // SPRITEENTITY_H
