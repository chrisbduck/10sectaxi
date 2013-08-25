//------------------------------------------------------------------------------
// SpriteEntity: Base class for interactive elements that have textures.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "spriteentity.h"

#include "camera.h"
#include "settings.h"
#include "texturemanager.h"
#include "useful.h"
#include "video.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL/SDL_surface.h>

//------------------------------------------------------------------------------

bool SpriteEntity::msStaticInitDone = false;

GLfloat SpriteEntity::msVertPosData[] =
{
	-0.5f,  0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	 0.5f,  0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f
};
GLfloat SpriteEntity::msVertUVData[] =
{
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f
};
GLuint SpriteEntity::msVertPosBufferID = 0;
GLuint SpriteEntity::msVertUVBufferID = 0;
GLuint SpriteEntity::msShaderProg = 0;
GLuint SpriteEntity::msPosAttributeID = 0, SpriteEntity::msUVAttributeID = 0;
GLuint SpriteEntity::msMatUniformID = 0, SpriteEntity::msColUniformID = 0, SpriteEntity::msTexUniformID = 0;
float SpriteEntity::msScreenScaleX = 1.0f, SpriteEntity::msScreenScaleY = 1.0f;

static const glm::vec3 kZAxis(0.0f, 0.0f, 1.0f);

//------------------------------------------------------------------------------

SpriteEntity::SpriteEntity(float lX, float lY) :
	Entity(lX, lY),
	mpTexture(nullptr),
	mRotationRad(0.0f),
	mRotationStartsFromUp(false),
	mBlendEnabled(true),
	mBehindCamera(false)
{
	for (int lIndex = 0; lIndex < sizeof(mColour) / sizeof(float); ++lIndex)
		mColour[lIndex] = 1.0f;
	
	if (!msStaticInitDone)
		staticInit();
}

//------------------------------------------------------------------------------

void SpriteEntity::staticInit()
{
	ASSERT(!msStaticInitDone);
	
	// Generate buffers for the positions and UVs
	GLuint lBufferIDs[2] = { 0, 0 };
	glGenBuffers(2, lBufferIDs);
	
	// Set up the positions buffer
	msVertPosBufferID = lBufferIDs[0];
	glBindBuffer(GL_ARRAY_BUFFER, msVertPosBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(msVertPosData), msVertPosData, GL_STATIC_DRAW);
	
	// Set up the UVs buffer
	msVertUVBufferID = lBufferIDs[1];
	glBindBuffer(GL_ARRAY_BUFFER, msVertUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(msVertUVData), msVertUVData, GL_STATIC_DRAW);
	
	// Prepare the shader programme
	msShaderProg = gVideo.buildShaderProgFromFiles(Settings::getString("shader/vertex"),
												   Settings::getString("shader/fragment"));
	
	if (msShaderProg != 0)
	{
		// Find the attributes and uniforms
		msPosAttributeID = glGetAttribLocation(msShaderProg, "a_VertPos");
		msUVAttributeID  = glGetAttribLocation(msShaderProg, "a_VertUV");
		msMatUniformID = glGetUniformLocation(msShaderProg, "u_Matrix");
		msColUniformID = glGetUniformLocation(msShaderProg, "u_Colour");
		msTexUniformID = glGetUniformLocation(msShaderProg, "u_Texture");
	}
	
	msScreenScaleX =  2.0f / Settings::getFloat("screen/width");
	msScreenScaleY = -2.0f / Settings::getFloat("screen/height");
	
	msStaticInitDone = true;
}

//------------------------------------------------------------------------------

void SpriteEntity::setTexture(Texture* lpTexture)
{
	mpTexture = lpTexture;
	setWidth(float(mpTexture->width()));
	setHeight(float(mpTexture->height()));
}

//------------------------------------------------------------------------------

void SpriteEntity::render() const
{
	Entity::render();
	
	if (msShaderProg == 0)
		return;
	glUseProgram(msShaderProg);
	
	float lAdjustedX = x();
	float lAdjustedY = y();
	if (!mBehindCamera)
	{
		if (!gpCamera->canSee(this))
			return;
		
		lAdjustedX -= gpCamera->offsetX();
		lAdjustedY -= gpCamera->offsetY();
	}
	
	// Set up the transformation matrix
	
	glm::mat4 lTransform(1.0f);		// identity
	lTransform = glm::translate(lTransform, glm::vec3(lAdjustedX * msScreenScaleX - 1.0f,
													  lAdjustedY * msScreenScaleY + 1.0f, 0.0f));
	lTransform = glm::scale(lTransform, glm::vec3(width() * msScreenScaleX, height() * msScreenScaleY, 1.0f));
	lTransform = glm::rotate(lTransform, fixedRotationRad(), kZAxis);
	glUniformMatrix4fv(msMatUniformID, 1, GL_FALSE, &lTransform[0][0]);
	
	// Colour
	glUniform4fv(msColUniformID, 1, mColour);
	
	// Texture
	mpTexture->activate();
	glUniform1i(msTexUniformID, 0);
	
	// Set up the positions
	glBindBuffer(GL_ARRAY_BUFFER, msVertPosBufferID);
	glVertexAttribPointer(msPosAttributeID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(msPosAttributeID);
	
	// Set up the UVs
	glBindBuffer(GL_ARRAY_BUFFER, msVertUVBufferID);
	glVertexAttribPointer(msUVAttributeID, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(msUVAttributeID);
	
	// Blending
	if (mBlendEnabled)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
		glDisable(GL_BLEND);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableVertexAttribArray(msUVAttributeID);
	glDisableVertexAttribArray(msPosAttributeID);
}

//------------------------------------------------------------------------------

float SpriteEntity::fixedRotationRad() const
{
	return mRotationStartsFromUp ? (-M_PI_OVER_2 - mRotationRad) : mRotationRad;
}

//------------------------------------------------------------------------------
// CollidableEntity
//------------------------------------------------------------------------------

CollidableEntity::CollidableEntity(float lX, float lY) :
	SpriteEntity(lX, lY),
	mUsesCircleCollisions(false)
{
}

//------------------------------------------------------------------------------

void CollidableEntity::getRotatedBoundingBox(float* lpLeftOut, float* lpTopOut, float* lpRightOut, float* lpBottomOut) const
{
	glm::mat4 lRotation = glm::rotate(glm::mat4(1.0f), fixedRotationRad(), kZAxis);
	
	glm::vec4 lTranslatedTopLeft = lRotation * glm::vec4(left(), top(), 0.0f, 1.0f);
	glm::vec4 lTranslatedBottomRight = lRotation * glm::vec4(right(), bottom(), 0.0f, 1.0f);
	
	*lpLeftOut   = min(lTranslatedTopLeft.x, lTranslatedBottomRight.x);
	*lpRightOut  = max(lTranslatedTopLeft.x, lTranslatedBottomRight.x);
	*lpTopOut    = min(lTranslatedTopLeft.y, lTranslatedBottomRight.y);
	*lpBottomOut = max(lTranslatedTopLeft.y, lTranslatedBottomRight.y);
}

//------------------------------------------------------------------------------

float CollidableEntity::bounceFactor() const
{
	return 1.0f;
}

//------------------------------------------------------------------------------

bool CollidableEntity::checkCollisionWith(CollidableEntity* lpOther)
{
	// For now, assume that we're colliding the player with a house
	if (name().find("player") == std::string::npos)
		return false;
	
	
	// Draw a line on each side of the player's car and see if it intersects the house rect
	
	static const std::vector<float> kCarXRange = Settings::getFloatVector("collision/car_x_range");
	static const std::vector<float> kCarYRange = Settings::getFloatVector("collision/car_y_range");
	static const float kCarCollLeft = getFloatParam(kCarXRange, 0);
	static const float kCarCollRight = getFloatParam(kCarXRange, 1);
	static const float kCarCollTop = getFloatParam(kCarYRange, 0);
	static const float kCarCollBottom = getFloatParam(kCarYRange, 1);
	
	// Get the translated corners of the car
	glm::mat4 lTransform = glm::translate(glm::mat4(1.0f), glm::vec3(x(), y(), 0.0f));
	lTransform = glm::rotate(lTransform, fixedRotationRad(), kZAxis);
	glm::vec4 lTransTL = lTransform * glm::vec4(kCarCollLeft - halfWidth(),  kCarCollTop - halfHeight(),    0.0f, 1.0f);
	glm::vec4 lTransTR = lTransform * glm::vec4(kCarCollRight - halfWidth(), kCarCollTop - halfHeight(),    0.0f, 1.0f);
	glm::vec4 lTransBL = lTransform * glm::vec4(kCarCollLeft - halfWidth(),  kCarCollBottom - halfHeight(), 0.0f, 1.0f);
	glm::vec4 lTransBR = lTransform * glm::vec4(kCarCollRight - halfWidth(), kCarCollBottom - halfHeight(), 0.0f, 1.0f);
	
	//printf("tl (%.1f, %.1f); tr (%.1f, %.1f); bl (%.1f, %.1f); br (%.1f, %.1f)\n",
	//	   lTransTL.x, lTransTL.y, lTransTR.x, lTransTR.y, lTransBL.x, lTransBL.y, lTransBR.x, lTransBR.y);
	
	// Check if any is in the rect
	bool lCollision = false;
	float lCollX, lCollY;
	if (lpOther->isPointInRect(lTransTL.x, lTransTL.y))
	{
		lCollision = true;
		lCollX = lTransTL.x;
		lCollY = lTransTL.y;
	}
	else if (lpOther->isPointInRect(lTransTR.x, lTransTR.y))
	{
		lCollision = true;
		lCollX = lTransTR.x;
		lCollY = lTransTR.y;
	}
	else if (lpOther->isPointInRect(lTransBL.x, lTransBL.y))
	{
		lCollision = true;
		lCollX = lTransBL.x;
		lCollY = lTransBL.y;
	}
	else if (lpOther->isPointInRect(lTransBR.x, lTransBR.y))
	{
		lCollision = true;
		lCollX = lTransBR.x;
		lCollY = lTransBR.y;
	}
	
	if (!lCollision)
		return false;
	
	//printf("%s collided with %s\n", name().c_str(), lpOther->name().c_str());
	
	float lBounceFactor = bounceFactor() * lpOther->bounceFactor();
	
	// Work out which is the nearest edge
	float lLeftOff = fabsf(lpOther->left() - lCollX);
	float lRightOff = fabsf(lpOther->right()- lCollX);
	float lTopOff = fabsf(lpOther->top() - lCollY);
	float lBottomOff = fabsf(lpOther->bottom() - lCollY);
	if (min(lLeftOff, lRightOff) < min(lTopOff, lBottomOff))
	{
		// Horizontal collision
		if (lLeftOff < lRightOff)
			setX(x() - lLeftOff);
		else
			setX(x() + lRightOff);
		setVelX(-velX() * lBounceFactor);
	}
	else
	{
		// Vertical collision
		if (lTopOff < lBottomOff)
			setY(y() - lTopOff);
		else
			setY(y() + lBottomOff);
		setVelY(-velY() * lBounceFactor);
	}
	
	
	
	/*int lTLX = int(lTransTL.x);
	int lTLY = int(lTransTL.y);
	int lBLX = int(lTransBL.x);
	int lBLY = int(lTransBL.y);
	int lTRX = int(lTransTR.x);
	int lTRY = int(lTransTR.y);
	int lBRX = int(lTransBR.x);
	int lBRY = int(lTransBR.y);
	
	SDL_Rect lHouseRect = { lpOther->left(), lpOther->top(), lpOther->width(), lpOther->height() };
	
	if (SDL_IntersectRectAndLine(lHouseRect, &lTLX, &lTLY, &lBLX, &lBLY)
			|| SDL_IntersectRectAndLine(lHouseRect, &lTRX, &lTRY, &lBRX, &lBRY))*/
	
/*
	if (lpOther->usesCircleCollisions())
	{
		printf("Skipping object with circle collisions\n");
		return false;
	}
	
	// The other object uses rect collisions.
	// Find the minimum x and y offsets to this object's centre
	float lXOffsetL = lpOther->left() - x();
	float lXOffsetR = lpOther->right() - x();
	float lXOffset = (fabsf(lXOffsetL) < fabsf(lXOffsetR)) ? lXOffsetL : lXOffsetR;
	float lYOffsetT = lpOther->top() - y();
	float lYOffsetB = lpOther->bottom() - y();
	float lYOffset = (fabsf(lYOffsetT) < fabsf(lYOffsetB)) ? lYOffsetT : lYOffsetB;
	//float lXOffset = min(fabsf(lpOther->left() - x()), fabsf(lpOther->right() - x()));
	//float lYOffset = min(fabsf(lpOther->top() - y()), fabsf(lpOther->bottom() - y()));
	// This gives us the displacement to the closest corner.
	
	// Compare the distance to this object's radius and we can check for a collision
	float lThisRadius = halfWidth();
	printf("x %.1f y %.1f => %.1f as compared to rad %.1f\n", lXOffset, lYOffset, sqrtf(lXOffset * lXOffset + lYOffset * lYOffset), lThisRadius);
	if (lXOffset * lXOffset + lYOffset * lYOffset >= lThisRadius * lThisRadius)
		return false;
	
	printf("%s collided with %s\n", name().c_str(), lpOther->name().c_str());
	
	float lBounceFactor = bounceFactor() * lpOther->bounceFactor();
	
	// We can just pick which of the offsets is smaller to apply the bounce-off force or stoppage
	if (fabsf(lXOffset) < fabsf(lYOffset))
		mVelX = -mVelX * lBounceFactor;
	else
		mVelY = -mVelY * lBounceFactor;
	
	//float lOffsetMag, lOffsetAngleRad;
	//getPolarFromRect(lXOffset, lYOffset, &lOffsetMag, &lOffsetAngleRad);
*/
	
	
	
	return true;
}

//------------------------------------------------------------------------------
