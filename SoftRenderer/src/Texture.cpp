#include "Texture.h"

Texture::Texture(SDL_Surface* src)
{
	SDL_Surface *tsurface = SDL_ConvertSurface(src, src->format, 0);
	if (tsurface == NULL)
	{
		printf("Texture create fail.");
	}
	else
	{
		surface.reset(tsurface, [](SDL_Surface* ptr) {
			if (ptr != NULL)
				SDL_FreeSurface(ptr);
		});
		width = surface->w;
		height = surface->h;
	}
}

Texture::Texture(const char* filepath)
{
	SDL_Surface* tsurface = IMG_Load(filepath);
	if (tsurface == NULL)
	{
		printf("Texture create fail.");
	}
	else
	{
		surface.reset(tsurface, [](SDL_Surface* ptr) {
			if (ptr != NULL)
				SDL_FreeSurface(ptr);
			});
		width = surface->w;
		height = surface->h;
	}
}


Texture::Texture(SDL_RWops* stream)
{
	SDL_Surface* tsurface = IMG_Load_RW(stream, 0);
	if (tsurface == NULL)
	{
		printf("Texture create from memory fail.");
	}
	else
	{
		surface.reset(tsurface, [](SDL_Surface* ptr) {
			if (ptr != NULL)
				SDL_FreeSurface(ptr);
			});
		width = surface->w;
		height = surface->h;
	}
}

Texture::Texture(const Texture& rhs)
{
	if (this != &rhs)
	{
		this->width = rhs.width;
		this->height = rhs.width;
		this->surface = rhs.surface;
	}
}

Texture& Texture::operator=(const Texture& rhs)
{
	if (this != &rhs)
	{
		this->width = rhs.width;
		this->height = rhs.width;
		this->surface = rhs.surface;
	}

	return *this;
}

Texture::~Texture()
{

}

Texture::Texture(Texture&& rhs) noexcept
{
	if (this != &rhs)
	{
		width = rhs.width;
		rhs.width = 0;

		height = rhs.height;
		rhs.height = 0;

		surface = std::move(rhs.surface);
		rhs.surface = nullptr;
	}
}
Texture& Texture::operator=(Texture&& rhs) noexcept
{
	if (this != &rhs)
	{
		width = rhs.width;
		rhs.width = 0;

		height = rhs.height;
		rhs.height = 0;

		surface = std::move(rhs.surface);
		rhs.surface = nullptr;
	}

	return *this;
}

Vector4f Texture::getColorFromUV(const float u, const float v) const
{
	int i = u * width;
	int j = (1 - v) * height;

	return getColor(i, j);
}

// i from left to right, j from top to bottom, line first
Vector4f Texture::getColor(const int i, const int j) const
{
	if (surface != NULL && i >= 0 && i < width && j >= 0 && j < height)
	{
		Uint8* pixels = static_cast<Uint8*>(surface->pixels);
		auto pixelSz = surface->format->BytesPerPixel;

		Uint32* p32 = reinterpret_cast<Uint32*>(pixels + (j * width + i) * pixelSz);

		Uint8 r, g, b, a;
		SDL_GetRGBA(*p32, surface->format, &r, &g, &b, &a);
		return Vector4f{ 
			static_cast<float>(r),
			static_cast<float>(g),
			static_cast<float>(b),
			static_cast<float>(a)
		};
	}
	else
	{
		return Vector4f{ 0.f, 0.f, 0.f, 0.f };
	}
}

// i from left to right, j from top to bottom, line first
void Texture::setColor(const int i, const int j, const Vector4f& color)
{
	if (surface != NULL && i >= 0 && i < width && j >= 0 && j < height)
	{
		Uint8* pixels = static_cast<Uint8*>(surface->pixels);
		auto pixelSz = surface->format->BytesPerPixel;

		auto mapCol = SDL_MapRGBA(surface->format, 
			MathUtility::clamp(color.x, 0.f, 255.0f),
			MathUtility::clamp(color.y, 0.f, 255.0f),
			MathUtility::clamp(color.z, 0.f, 255.0f),
			MathUtility::clamp(color.w, 0.f, 255.0f)
		);
		// pixels[(j * width + i) * pixelSz] = mapCol;
		Uint32* p32 = reinterpret_cast<Uint32 *>(pixels + (j * width + i) * pixelSz);
		*p32 = mapCol;

	}
}

void Texture::clear(const Vector4f& color)
{
	if (surface != NULL)
	{
		Uint32* pixels = static_cast<Uint32*>(surface->pixels);
		auto mapCol = SDL_MapRGBA(surface->format,
			MathUtility::clamp(color.x, 0.f, 255.0f),
			MathUtility::clamp(color.y, 0.f, 255.0f),
			MathUtility::clamp(color.z, 0.f, 255.0f),
			MathUtility::clamp(color.w, 0.f, 255.0f)
		);
		std::fill_n(pixels, surface->w * surface->h, mapCol);
	}
}
