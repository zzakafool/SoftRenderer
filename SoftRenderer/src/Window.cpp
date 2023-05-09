#include "Window.h"
#include <algorithm>
Window::Window(const unsigned int w, const unsigned int h) : width(w), height(h)
{
	if (!hasInited)
		init();
}

Window::~Window()
{
	uninit();
}

void Window::init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error : %s\n", SDL_GetError());
		return;
	}

	window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return;
	}

	screenSurface = SDL_GetWindowSurface(window);
	renderSurface = SDL_ConvertSurface(screenSurface, screenSurface->format, 0);

	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
	SDL_UpdateWindowSurface(window);

	hasInited = true;
}

void Window::loop()
{
	if (!hasInited)
		init();

	Uint32* pixels = static_cast<Uint32*>(renderSurface->pixels);
	int pixelCount = renderSurface->pitch / 4 * height;

	auto newColor = SDL_MapRGBA(renderSurface->format, 255, 0, 0, 0);
	for (int i = 0; i < pixelCount; ++i)
	{
		int x = i / width;
		int y = i % width;

		if (x == y)
			pixels[i] = newColor;
	}

	SDL_Event e;
	bool quit = false;
	while (quit == false)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				quit = true;
		}

		std::fill_n(pixels, pixelCount, SDL_MapRGBA(renderSurface->format, 0, 0xFF, 0, 0));
		SDL_BlitSurface(renderSurface, NULL, screenSurface, NULL);

		SDL_UpdateWindowSurface(window);
	}
}

void Window::uninit()
{
	if (!hasInited)
		return;

	window = NULL;
	screenSurface = NULL;
	renderSurface = NULL;

	SDL_FreeSurface(renderSurface);
	SDL_DestroyWindow(window);
	SDL_Quit();
}