//------------------------------------------------------------------------------
// Video: Rendering code.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef VIDEO_H
#define VIDEO_H

#include <GLES2/gl2.h>
#include <string>
#include <unordered_set>

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
	
	// There's no need to delete programmes built with these; the video object will do so on shut down
	GLuint buildShaderProg(const char* lpVertShaderSrc, const char* lpFragShaderSrc);
	GLuint buildShaderProgFromFiles(const std::string& lrVertShaderFileName, const std::string& lrFragShaderFileName);
	
private:
	
	enum ShaderType { kVertexShader, kFragmentShader };
	GLuint loadShader(const char* lpSourceText, ShaderType lType);
	
	bool			mInitialised;
	SDL_Surface*	mpDisplaySurface;
	
	std::unordered_set<GLuint> mShaderProgSet;
};

extern Video gVideo;

//------------------------------------------------------------------------------

#endif // VIDEO_H
