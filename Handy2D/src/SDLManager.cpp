#include "SDLManager.h"
#include <chrono>
#include <array>
#include <iostream>
#include "RenderLayer.h"
#include "Input.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "PaintedImage.h"
#include "Texture.h"
#include "Time.h"



class SDLEventHandler
{
public:
	static void Handle(const SDL_Event& Event)
	{
		switch (Event.type)
		{
		case SDL_KEYDOWN:
		{
			if (!Event.key.repeat)
			{
				int key = Event.key.keysym.scancode;
				SToggleState& State = Input::GetButton(key);
				State.active = true;
				State.downtime = 0.0f;
				State.pressed = true;
				Input::m_aJustPressed.insert(key);
				//std::cout << key << "\n";
			}
		}
		break;
		case SDL_KEYUP:
		{
			int key = Event.key.keysym.scancode;
			SToggleState& State = Input::GetButton(key);
			State.active = false;
			State.released = true;
			Input::m_aJustReleased.insert(key);
		}
		break;
		case SDL_MOUSEMOTION:
			Input::m_vMousePos.x = Event.motion.x;
			Input::m_vMousePos.y = Event.motion.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
		{
			int key = -1 * Event.button.button;
			SToggleState& State = Input::GetButton(key);
			State.active = true;
			State.downtime = 0.0f;
			State.pressed = true;
			Input::m_aJustPressed.insert(key);
		}
		break;
		case SDL_MOUSEBUTTONUP:
		{
			int key = -1 * Event.button.button;
			SToggleState& State = Input::GetButton(key);
			State.active = false;
			State.released = true;
			Input::m_aJustReleased.insert(key);
		}
		break;
		case SDL_MOUSEWHEEL:
			break;
		default:
			break;
		}
	}
};


SDLManager SDLManager::Instance;
int SDLManager::drawnum = 0;
int SDLManager::spritenum = 0;

SDL_Renderer* SDLManager::GetRenderer() { return m_pRenderer; }

void SDLManager::Start(SWindowParams params, CGameLogic* pGameLogic)
{
	Instance.m_WindowParams = params;
	Instance.m_pGameLogic = pGameLogic;

	if (Instance.Init())
	{
		Instance.MainLoop();
	}
}

void SDLManager::SetWindowSize(IntVec size)
{
	SDL_SetWindowSize(Instance.m_pWindow, size.x, size.y);
}
void SDLManager::SetWindowMinSize(IntVec size)
{
	SDL_SetWindowMinimumSize(Instance.m_pWindow, size.x, size.y);
}
void SDLManager::SetWindowMaxSize(IntVec size)
{
	SDL_SetWindowMaximumSize(Instance.m_pWindow, size.x, size.y);
}


bool SDLManager::Init()
{

	try
	{

		if (SDL_Init(SDL_INIT_VIDEO) < 0) throw std::runtime_error("SDL could not initialize!");

		Uint32 flags = SDL_WINDOW_SHOWN;
		if (m_WindowParams.bFullscreen) flags = flags | SDL_WINDOW_FULLSCREEN;
		if (m_WindowParams.bBorderless) flags = flags | SDL_WINDOW_BORDERLESS;
		if (m_WindowParams.bMaximized) flags = flags | SDL_WINDOW_MAXIMIZED;	
		if (m_WindowParams.bMinimized) flags = flags | SDL_WINDOW_MINIMIZED;
		if (m_WindowParams.bResizeAble) flags = flags | SDL_WINDOW_RESIZABLE;

		m_pWindow = SDL_CreateWindow(m_WindowParams.sTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_WindowParams.nWidth, m_WindowParams.nHeight, flags);
		if (m_pWindow == NULL)
		{
			std::string error = "Window could not be created! error:";
			error += SDL_GetError();
			throw std::runtime_error(error);
		}

		m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
		if (m_pRenderer == NULL)
		{
			std::string error = "Renderer could not be created! error:";
			error += SDL_GetError();
			throw std::runtime_error(error);
		}
		SDL_SetRenderDrawColor(m_pRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

		int imgFlags = IMG_INIT_PNG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			std::string error = "SDL_image could not initialize! error:";
			error += IMG_GetError();
			throw std::runtime_error(error);
		}

		if (TTF_Init() == -1)
		{
			std::string error = "TTF init error! error:";
			error += TTF_GetError();
			throw std::runtime_error(error);
		}

		if (!LoadMedia()) throw std::runtime_error("Failed to load media!");
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << "\n";
		return false;
	}
	

	/*
	std::array<int, 800> aHeights;
	for (int x = 0; x < 800; ++x)
	{
		aHeights[x] = (int)( 250 + sin(x / (double)50) * 120);
	}

	m_pGenerated = new PaintedImage(800, 600);
	m_pGenerated->Fill([aHeights](int x, int y) -> int {
		return y < aHeights[x] ? 0x22ff44ff : 0x77bbffff;
	});
	*/
	return true;

}


bool SDLManager::LoadMedia()
{	//Loading success flag
	bool success = true;

	/* default font is deprecated
	m_pDefaultFont = TTF_OpenFont("arial.ttf", 18);
	if (m_pDefaultFont == nullptr)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	*/

	success &= m_pGameLogic->Load();

	return success;
}

void SDLManager::Close()
{
	//delete layers
	for (auto& Layer : m_aRenderLayers)
	{
		delete(Layer);
	}
	m_aRenderLayers.clear();

	//unload all image
	CTexture::UnloadTextures();

	SDL_DestroyRenderer(m_pRenderer);
	m_pRenderer = nullptr;

	SDL_DestroyWindow(m_pWindow);
	m_pWindow = nullptr;

	delete(m_pGameLogic);
	m_pGameLogic = nullptr;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void SDLManager::MainLoop()
{

	Time::Init();

	while (m_bRunning)
	{
		Time::FrameBegin();

		SDL_Event e;
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				m_bRunning = false;
			}

			SDLEventHandler::Handle(e);
		}
		
		m_bRunning &= m_pGameLogic->m_bRunning;

		RunFrame();

		Time::FrameEnd();
		Input::Update();
	}

	Time::Close();

	Close();
}


void SDLManager::RunFrame()
{
	m_pGameLogic->Update();

	drawnum = 0;

	SDL_RenderClear(m_pRenderer);

	for (auto&& Layer : m_aRenderLayers)
	{
		Layer->Render();
		Layer->HandleEvents();
	}	

	SDL_RenderPresent(m_pRenderer);
}


Vec SDLManager::GetSize()
{
	return Vec({ (float) Instance.m_WindowParams.nWidth, (float)Instance.m_WindowParams.nHeight });
}

void SDLManager::AddLayer(CRenderLayer * pLayer)
{
	m_aRenderLayers.push_back(pLayer);
}
