#include "SDLManager.h"
#include <Windows.h>
#include "Breakout.h"



int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{

	SWindowParams Params;
	Params.nWidth = 960;
	Params.nHeight = 600;
	Params.sTitle = "Strategy Breakout";
	Params.bResizeAble = false;
	Params.bMaximized = true;
	//Params.bFullscreen = true;

	SDLManager::clearcolor[0] = 30;
	SDLManager::clearcolor[1] = 30;
	SDLManager::clearcolor[2] = 30;

	CBreakoutGame* pGame = new CBreakoutGame();
	SDLManager::Start(Params, pGame);
	//delete pGame;

	return 0;
}