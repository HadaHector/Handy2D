#pragma once
#include <stdio.h>
#include "GameLogic.h"

//these could go to stdafx
#include "Geometry.h"
#include "Time.h"
#include <map>
#include <list>
#include <string>

class CRenderLayer;
class CTexture;
class CSprite;


struct SWindowParams
{
	std::string sTitle;
	bool bFullscreen = false;
	bool bBorderless = false;
	bool bMaximized = false;
	bool bMinimized = false;
	bool bResizeAble = false;
	int nWidth, nHeight, nLeft, nRight;
};


class SDLManager
{
public:
	static SDLManager Instance;
	static void Start(SWindowParams Params, CGameLogic* pGameLogic);
	static void SetWindowSize(IntVec size);
	static void SetWindowMinSize(IntVec size);
	static void SetWindowMaxSize(IntVec size);
	static void SetClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	static IntVec GetSize();
	static CGameLogic* GetGameLogic();
	static int drawnum;
	static int spritenum;

	static unsigned char clearcolor[4];

	struct SDL_Renderer* GetRenderer();
	void AddLayer(CRenderLayer* pLayer);
	std::list<CRenderLayer*>& GetRenderLayers() { return m_aRenderLayers; }

	int m_nWidth, m_nHeight;

private:
	
	bool Init();
	bool LoadMedia();
	void Close();

	bool m_bRunning = true;
	void MainLoop();
	void RunFrame();

	SDLManager() {};
	~SDLManager() {};

	//The window we'll be rendering to
	struct SDL_Window* m_pWindow = nullptr;

	//The surface contained by the window
	struct SDL_Surface* m_pScreenSurface = nullptr;
	struct SDL_Renderer* m_pRenderer = NULL;

	std::list<CRenderLayer*> m_aRenderLayers;

	CGameLogic* m_pGameLogic = nullptr;

	SWindowParams m_WindowParams;
};

