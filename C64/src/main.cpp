#include "SDLManager.h"
#include <Windows.h>
#include "Breakout.h"



int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{

	SWindowParams Params;
	Params.nWidth = 960;
	Params.nHeight = 600;
	Params.sTitle = "C64 display";
	Params.bResizeAble = false;
	Params.bMaximized = false;
	//Params.bFullscreen = true;

	CBreakoutGame* pGame = new CBreakoutGame();
	SDLManager::Start(Params, pGame);
	//delete pGame;

	return 0;
}