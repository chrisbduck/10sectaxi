//------------------------------------------------------------------------------
// SpriteEntity: Base class for interactive elements that have textures.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef SPRITEENTITY_H
#define SPRITEENTITY_H

#include "entity.h"

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
	
	float rotation() const { return mRotationRad; }
	void setRotation(float lRotation) { mRotationRad = lRotation; }
	
	void setBlendEnabled(bool lEnabled) { mBlendEnabled = lEnabled; }
	
protected:
	void setRotationStartsFromUp(bool lEnabled) { mRotationStartsFromUp = lEnabled; }	// for car sprite, etc
	
private:
	
	static void staticInit();
	
	
	Texture* mpTexture;
	float mRotationRad;
	bool mRotationStartsFromUp;
	bool mBlendEnabled;
	
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

#endif // SPRITEENTITY_H
