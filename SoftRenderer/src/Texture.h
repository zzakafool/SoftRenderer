#ifndef M_TEXTURE_H
#define M_TEXTURE_H

#include <SDL.h>
#include <SDL_image.h>
#include <memory>
#include "Math.h"

class Texture
{
public:
	int width;
	int height;

	Texture() : width(0), height(0), surface(NULL) {};
	Texture(SDL_Surface* src);			// create a new one from a exact surface
	Texture(const char* filepath);		// load from file
	Texture(SDL_RWops* stream);			// load from memory

	// just move
	Texture(const Texture& rhs);
	Texture& operator=(const Texture& rhs);

	Texture(Texture&& rhs) noexcept;
	Texture& operator=(Texture&& rhs) noexcept;

	Vector4f getColor(const int x, const int y) const;
	Vector4f getColorFromUV(const float u, const float v) const;
	void setColor(const int x, const int y, const Vector4f& color);
	void clear(const Vector4f& color);

	SDL_Surface* getRawSurface() { return surface.get(); };

	~Texture();
private:
	std::shared_ptr<SDL_Surface> surface;
};

#endif