#include "SDLManager.h"
#include "C64Renderer.h"
#include "Input.h"
#include <Windows.h>
#include <random>




class CBreakoutGame: public CGameLogic
{
public:
	// Inherited via CGameLogic
	virtual bool Load() override
	{
		Time::target_fps = 120;

		m_pC64 = new CC64RenderLayer(IntRect(0, 0, 1280, 800));
		SDLManager::Instance.AddLayer(m_pC64);

		SC64Char Char;
		Char.cBG = EC64Color::cyan;
		m_pC64->SetCharacter(1, 1, Char);

		m_pC64->Redraw();
		return true;
	}

	virtual void Update() override
	{
		m_dFrametimer += Time::frame;
		if (m_dFrametimer < 0.02) return;
		
		m_dFrametimer = 0;

		iTest++;


		SC64Char Char;
		Char.cBG = EC64Color::cyan;
		m_pC64->SetCharacter(iTest%40, 1, Char);
	}

	int iTest = 0;
	double m_dFrametimer;

	CC64RenderLayer* m_pC64;
};





int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{

	SWindowParams Params;
	Params.nWidth = 1280;
	Params.nHeight = 800;
	Params.sTitle = "Strategy Breakout";
	Params.bResizeAble = false;
	Params.bMaximized = false;

	CBreakoutGame* pGame = new CBreakoutGame();
	SDLManager::Start(Params, pGame);
	//delete pGame;

	return 0;
}