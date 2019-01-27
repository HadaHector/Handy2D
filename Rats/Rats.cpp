#include "SDLManager.h"
#include "RatGame.h"

#include <Windows.h>



int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{

	SWindowParams Params;
	Params.nWidth = 800;
	Params.nHeight = 600;
	Params.sTitle = "Rat Tunnels";
	Params.bResizeAble = true;
	Params.bMaximized = true;
	
	CRatGame* pGame = new CRatGame();
	SDLManager::Start(Params, pGame);
	
	return 0;
}