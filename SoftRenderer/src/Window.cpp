#include "Window.h"
#include <algorithm>

const float MY_PI = 3.1415926;

Window::Window(const unsigned int w, const unsigned int h) : width(w), height(h), initTime(std::chrono::system_clock::now())
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

	//Initialize PNG loading
	int imgFlags = IMG_INIT_JPG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return;
	}

	window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return;
	}

	screenSurface = SDL_GetWindowSurface(window);
	renderer = Renderer(screenSurface);
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
	SDL_UpdateWindowSurface(window);
	SDL_ShowCursor(false);

	hasInited = true;
}

DrawParams Window::initData()
{
	DrawParams dp;
	VertexShaderParams vsParam;

	// this->model.load("model/nanosuit/nanosuit.obj");
	// this->model.load("model/spot_triangulated_good.obj");
	// this->model.load("model/jotaro.obj");
	this->model.load("model/Bboy Hip Hop Move.fbx");

	renderer.setVertexShader(vectexShader);
	renderer.setFragmentShader(fragmentShader);

	dp.vsParams = vsParam;

	FragmentShaderParams fsParam;
	fsParam.textureVec = this->model.textureVec;

	dp.fsParams = fsParam;

	for (int i = 0; i < this->model.meshes.size(); ++i)
	{
		this->model.meshes[i].addToRenderer(&renderer);
		this->model.meshes[i].setDrawParams(dp);
	}

	dp.fsParams.lights.push_back(Light(Vector3f{ 20.f, 20.f, 100.f }, Vector3f{ 800.f, 800.f, 800.f }));
	dp.fsParams.lights.push_back(Light(Vector3f{ -20.f, 20.f, 0.f }, Vector3f{ 800.f, 800.f, 800.f }));
	dp.fsParams.lights.push_back(Light(Vector3f{ -20.f, -20.f, 0.f }, Vector3f{ 800.f, 800.f, 800.f }));

	return dp;
}

void Window::loop()
{
	if (!hasInited)
		init();

	DrawParams dp = initData();

	SDL_Event e;
	bool quit = false;

	float allangle = 0.f;

	while (quit == false)
	{
		float yawDiff = 0.f;
		float pitchDiff = 0.f;

		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_MOUSEMOTION:
					// std::cout << "xrel : " << e.motion.xrel << "yrel : " << e.motion.yrel << std::endl;
					yawDiff = -e.motion.xrel / 20.f;
					pitchDiff = -e.motion.yrel / 20.f;
					break;
				case SDL_KEYDOWN:
					if (e.key.keysym.sym == SDLK_w)
						camera.MoveFwd(1);
					else if (e.key.keysym.sym == SDLK_s)
						camera.MoveFwd(-1);
					else if (e.key.keysym.sym == SDLK_d)
						camera.MoveRight(1);
					else if (e.key.keysym.sym == SDLK_a)
						camera.MoveRight(-1);
					else if (e.key.keysym.sym == SDLK_q)
						camera.MoveUp(1);
					else if (e.key.keysym.sym == SDLK_e)
						camera.MoveUp(-1);
					else if (e.key.keysym.sym == SDLK_SPACE)
						dp.type = static_cast<Primitive>((static_cast<int>(dp.type) + 1)% static_cast<int>(Primitive::MAX_SIZE));
					break;
			}
		}

		// ------------------------------------
		Matrix4f rotation;
		allangle += 3.0f;
		auto angle = allangle * MY_PI / 180.f;
		rotation = { cos(angle), 0, sin(angle), 0,
						0, 1, 0, 0,
						-sin(angle), 0, cos(angle), 0,
						0, 0, 0, 1 };

		Matrix4f scale;
		scale = { 0.1, 0, 0, 0,
				0, 0.1, 0, 0,
				0, 0, 0.1, 0,
				0, 0, 0, 1 };

		Matrix4f translate;
		translate = { 1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1 };

		Matrix4f model = translate * rotation * scale;
		camera.addYaw(yawDiff);
		camera.addPitch(pitchDiff);
		Matrix4f view = camera.getViewMatrix();
		Matrix4f projection = MathUtility::getPerspctiveMatrix(45, width *1.0f / (height*1.0f) , 0.1, 50.f);

		dp.vsParams.p = projection;
		dp.vsParams.mv = view * model;
		dp.vsParams.mv_i_T = (view * model).inverse().transpose();
		dp.vsParams.zNear = 0.1f;
		dp.vsParams.zFar = 50.f;
		// ----------------------------------------
		
		renderer.clearColor(Vector4f{ 0.0f, 0.0f, 0.0f,0.0f });
		renderer.clearZ();

		auto diff = std::chrono::system_clock::now() - initTime;
		float animSec = std::chrono::duration_cast<std::chrono::microseconds>(diff).count() * 1.0 / 1000000;
		for (int i = 0; i < this->model.meshes.size(); ++i)
		{
			this->model.meshes[i].setDrawParams(dp, animSec);
			renderer.draw(dp);
		}
		SDL_BlitSurface(renderer.getRenderTexture().getRawSurface(), NULL, screenSurface, NULL);

		SDL_UpdateWindowSurface(window);
	}
}

void Window::uninit()
{
	if (!hasInited)
		return;

	window = NULL;
	screenSurface = NULL;

	SDL_DestroyWindow(window);
	SDL_Quit();
}