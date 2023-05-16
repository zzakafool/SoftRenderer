#ifndef M_WINDOW_H
#define M_WINDOW_H

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include "Texture.h"
#include "Renderer.h"
#include "vertexShader.h"
#include "fragmentShader.h"
#include "Model.h"
#include "Camera.h"
#include <chrono>

class Window
{
public:
	Window(const unsigned int w = 800, const unsigned int h = 600);
	void loop();
	virtual ~Window();
private:
	void init();
	DrawParams initData();
	void uninit();
private:
	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;
	Renderer renderer;
	Model model;

	Camera camera;

	bool hasInited = false;
	unsigned int width = 700;
	unsigned int height = 700;

	std::chrono::system_clock::time_point initTime;
};

#endif