//------------------------------------------------------------------------------
// Entity: Base class for interactive elements.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>

//------------------------------------------------------------------------------

struct SDL_Surface;

//------------------------------------------------------------------------------

class Entity
{
public:
	Entity(float lX, float lY);
	virtual ~Entity() {}
	
	virtual void update(float lTimeDeltaSec);
	virtual void render() const {}
	
	virtual const char* type() const { return "entity"; }
	
	typedef Entity* (*FactoryFn)(const std::vector<std::string>& lrParameters);
	
	const std::string& name() const { return mName; }
	void setName(const std::string& lrName) { mName = lrName; }
	
	float x() const { return mX; }
	float y() const { return mY; }
	void setX(float lX) { mX = lX; }
	void setY(float lY) { mY = lY; }
	void setPos(float lX, float lY) { mX = lX; mY = lY; }
	
	float velX() const { return mVelX; }
	float velY() const { return mVelY; }
	void setVelX(float lVelX) { mVelX = lVelX; }
	void setVelY(float lVelY) { mVelY = lVelY; }
	void setVel(float lVelX, float lVelY) { mVelX = lVelX; mVelY = lVelY; }
	
	void getSpeedAndDir(float* lpSpeedOut, float* lpDirRadOut) const;
	float speed() const;
	
	float width() const			{ return mWidth; }
	float height() const		{ return mHeight; }
	float halfWidth() const		{ return mWidth * 0.5f; }
	float halfHeight() const	{ return mHeight * 0.5f; }
	
	float left() const		{ return mX - mWidth * 0.5f; }
	float right() const		{ return mX	+ mWidth * 0.5f; }
	float top() const		{ return mY - mHeight * 0.5f; }
	float bottom() const	{ return mY + mHeight * 0.5f; }
	
protected:
	
	void setWidth(float lWidth)		{ mWidth = lWidth; }
	void setHeight(float lHeight)	{ mHeight = lHeight; }
	void setSize(float lWidth, float lHeight) { mWidth = lWidth; mHeight = lHeight; }
	
	float mX;
	float mY;
	float mVelX;
	float mVelY;
	
private:
	std::string mName;
	float mWidth;
	float mHeight;
};

//------------------------------------------------------------------------------

#endif // ENTITY_H
