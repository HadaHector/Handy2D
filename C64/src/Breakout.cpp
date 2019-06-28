#include "SDLManager.h"
#include "Breakout.h"
#include "Input.h"

// Inherited via CGameLogic

bool CBreakoutGame::Load()
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
			m_pC64->SetCharacter(j, i + 1, Char);
		}
	}

	WriteText("COMMODORE 64 DISPLAY", 0, 0);
	m_pC64->Redraw();

	m_vBallPos = { 100,50 };
	m_vBallSpeed = { 5,5 };
	return true;
}

void CBreakoutGame::WriteText(const std::string & str, int x, int y)
{
	for (int i = 0; i < str.size(); ++i)
	{
		if (x + i > 39) return;
		SC64Char Char = m_pC64->GetCharacter(x + i, y);
		Char.SetCharacter(str[i], false);
		m_pC64->SetCharacter(x + i, y, Char);
	}
}

void CBreakoutGame::Update()
{
	m_dFrametimer += Time::frame;
	if (m_dFrametimer < 0.02) return;


	UpdateBall();
	m_pC64->Redraw();
	m_dFrametimer = 0;

	iTest++;

}


void CBreakoutGame::UpdateBall()
{
	IntVec vOldPos = m_vBallPos;
	m_vBallPos += m_vBallSpeed;
	int over = m_vBallPos.y - 24 * 8 - 4;
	if (over > 0)
	{
		m_vBallPos.y -= over;
		m_vBallSpeed.y *= -1;
	}

	if (m_vBallPos.y < 0)
	{
		m_vBallPos.y -= m_vBallPos.y;
		m_vBallSpeed.y *= -1;
	}

	over = m_vBallPos.x - 39 * 8 - 4;
	if (over > 0)
	{
		m_vBallPos.x -= over;
		m_vBallSpeed.x *= -1;
	}

	if (m_vBallPos.x < 0)
	{
		m_vBallPos.x -= m_vBallPos.x;
		m_vBallSpeed.x *= -1;
	}
	
	for (int x = 0; x < 4; ++x)
	{
		for (int y = 0; y < 4; ++y)
		{
			m_pC64->SetPixel(vOldPos.x + x, vOldPos.y + y, false);
		}
	}
	
	for (int x = 0; x < 4; ++x)
	{
		for (int y = 0; y < 4; ++y)
		{
			if (x == 0 && y == 0) continue;
			if (x == 0 && y == 3) continue;
			if (x == 3 && y == 0) continue;
			if (x == 3 && y == 3) continue;
			m_pC64->SetPixel(m_vBallPos.x + x, m_vBallPos.y + y, true);
		}
	}
	
}