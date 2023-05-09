#ifndef M_WINDOW_H
#define M_WINDOW_H

#include <SDL.h>
#include <stdio.h>

class Window
{
public:
	Window(const unsigned int w = 800, const unsigned int h = 600);
	void loop();
	virtual ~Window();
private:
	void init();
	void uninit();
private:
	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;
	SDL_Surface* renderSurface = NULL;
	bool hasInited = false;
	unsigned int width = 800;
	unsigned int height = 600;
};

#endif