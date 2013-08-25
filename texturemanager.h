//------------------------------------------------------------------------------
// TextureManager: Loads textures for use in various places.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <GLES2/gl2.h>
#include <string>
#include <unordered_map>

//------------------------------------------------------------------------------

struct SDL_Surface;
class Texture;

//------------------------------------------------------------------------------

class TextureManager
{
public:
	TextureManager();
	~TextureManager();
	
	void init();
	void shutDown();
	
	Texture* load(const std::string& lrFileName);
	
private:
	std::unordered_map<std::string, Texture*> mTextures;
};

extern TextureManager gTextureManager;

//------------------------------------------------------------------------------

class Texture
{
public:
	enum FilteringType { kNearest, kLinear };
	
	Texture(SDL_Surface* lpSurface, FilteringType lFilteringType = kLinear);	// the texture takes ownership of this surface
	~Texture();
	
	void activate(GLenum lTextureStage = GL_TEXTURE0);
	
	int width() const;
	int height() const;
	
private:
	SDL_Surface*	mpSurface;		// the texture owns this surface
	GLuint			mTexID;
};

//------------------------------------------------------------------------------

#endif // TEXTUREMANAGER_H
