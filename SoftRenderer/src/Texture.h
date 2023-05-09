#ifndef M_TEXTURE_H
#define M_TEXTURE_H

#include <SDL.h>

class Texture
{
public:
	int width;
	int height;

	Texture(SDL_Surface* src);
	Texture(const char* filepath);

	

private:
	SDL_Surface* surface;
};

#endif