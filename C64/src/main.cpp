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

		for (int i = 0; i < 16; ++i)
		{
			for (int j = 0; j < 40; ++j)
			{
				SC64Char Char;
				Char.cBG = (EC64Color)i;
				m_pC64->SetCharacter(j, i+1, Char);
			}
		}
		
		WriteText("COMMODORE 64 DISPLAY", 0, 0);
		m_pC64->Redraw();
		return true;
	}

	void WriteText(const std::string& str, int x, int y)
	{
		for (int i = 0; i < str.size(); ++i)
		{
			if (x + i > 39) return;
			SC64Char Char = m_pC64->GetCharacter(x+i, y);
			Char.SetCharacter(str[i], false);
			m_pC64->SetCharacter(x + i, y, Char);
		}
	}

	virtual void Update() override
	{
		m_dFrametimer += Time::frame;
		if (m_dFrametimer < 0.02) return;
		
		m_dFrametimer = 0;

		iTest++;

		
		
		
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
	Params.sTitle = "C64 display";
	Params.bResizeAble = false;
	Params.bMaximized = false;
	//Params.bFullscreen = true;

	CBreakoutGame* pGame = new CBreakoutGame();
	SDLManager::Start(Params, pGame);
	//delete pGame;

	return 0;
}