
#include "SDLManager.h"
#include "SplashGame.h"

#include <Windows.h>

//#include "vld.h"



int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{

	SWindowParams Params;
	Params.nWidth = 800;
	Params.nHeight = 600;
	Params.sTitle = "Splash Tycoon";
	Params.bResizeAble = true;
	Params.bMaximized = true;
	
	CSplashGame* pGame = new CSplashGame();
	SDLManager::Start(Params, pGame);

	return 0;
}