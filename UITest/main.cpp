#include "SDLManager.h"
#include "GuiLayer.h"
#include <Windows.h>






class CUITest : public CGameLogic
{
	virtual bool Load() override
	{
		Image = PNEW(CGuiImage);


		return true;
	}
	virtual void Update() override
	{
	}

	PGuiElement Image;
};





int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{

	SWindowParams Params;
	Params.nWidth = 800;
	Params.nHeight = 600;
	Params.sTitle = "Rat Tunnels";
	Params.bResizeAble = true;
	Params.bMaximized = true;

	CUITest* pGame = new CUITest();
	SDLManager::Start(Params, pGame);
	delete pGame;

	return 0;
}