//------------------------------------------------------------------------------
// Video: Rendering code.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef VIDEO_H
#define VIDEO_H

#include <GLES2/gl2.h>
#include <string>

//------------------------------------------------------------------------------

struct SDL_Surface;

//------------------------------------------------------------------------------

class Video
{
public:
	Video();
	~Video();
	
	bool init(int lDisplayWidth, int lDisplayHeight);
	void shutDown();
	
	void clear();
	void flip();
	
	SDL_Surface* getDisplaySurface() const { return mpDisplaySurface; }
	
private:
	
	enum ShaderType { kVertexShader, kFragmentShader };
	GLuint loadShader(const char* lpSourceText, ShaderType lType);
	GLuint buildProg(const char* lpVertShaderSrc, const char* lpFragShaderSrc);
	
	bool			mInitialised;
	SDL_Surface*	mpDisplaySurface;
	GLuint			mShaderProg;
};

extern Video gVideo;

//------------------------------------------------------------------------------

#endif // VIDEO_H
