//------------------------------------------------------------------------------
// TextureManager: Loads textures for use in various places.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <string>
#include <unordered_map>

//------------------------------------------------------------------------------

struct SDL_Surface;

//------------------------------------------------------------------------------

class TextureManager
{
public:
	TextureManager();
	~TextureManager();
	
	void init();
	void shutDown();
	
	SDL_Surface* load(const std::string& lrFileName);
	
private:
	std::unordered_map<std::string, SDL_Surface*> mTextures;
};

extern TextureManager gTextureManager;

//------------------------------------------------------------------------------

#endif // TEXTUREMANAGER_H
