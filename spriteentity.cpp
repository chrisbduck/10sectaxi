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

//------------------------------------------------------------------------------

SpriteEntity::SpriteEntity(float lX, float lY) :
	Entity(lX, lY),
	mpTexture(nullptr),
	mRotationRad(0.0f),
	mRotationStartsFromUp(false)
{
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
	
	if (!gpCamera->canSee(this))
		return;
	
	float lAdjustedX = x() - gpCamera->offsetX();
	float lAdjustedY = y() - gpCamera->offsetY();
	
	// Set up the transformation matrix
	
	float lFixedRotationRad = mRotationStartsFromUp ? (-M_PI_OVER_2 - mRotationRad) : mRotationRad;
	
	glm::mat4 lTransform(1.0f);		// identity
	lTransform = glm::translate(lTransform, glm::vec3(lAdjustedX * msScreenScaleX - 1.0f,
													  lAdjustedY * msScreenScaleY + 1.0f, 0.0f));
	lTransform = glm::rotate(lTransform, lFixedRotationRad, glm::vec3(0.0f, 0.0f, 1.0f));
	lTransform = glm::scale(lTransform, glm::vec3(width() * msScreenScaleX, height() * msScreenScaleY, 1.0f));
	glUniformMatrix4fv(msMatUniformID, 1, GL_FALSE, &lTransform[0][0]);
	
	// Colour
	float lCol[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(msColUniformID, 1, lCol);
	
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
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableVertexAttribArray(msUVAttributeID);
	glDisableVertexAttribArray(msPosAttributeID);
}

//------------------------------------------------------------------------------
