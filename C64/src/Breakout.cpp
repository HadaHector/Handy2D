#include "SDLManager.h"
#include "Breakout.h"
#include "Input.h"

#include <fstream>
#include <sstream>
#include <algorithm>

#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 16

//#define NO_FAIL

std::vector<SC64Char> g_aLoadedChars;
#define BRICK_1_LEFT 0
#define BRICK_1_RIGHT 1
#define UI_BALL_LEFT 2
#define UI_BALL_RIGHT 3
#define UI_BAT_LEFT 4
#define UI_BAT_RIGHT 5
#define UI_TOP 6

void CBreakoutGame::LoadChars()
{
	g_aLoadedChars.push_back(LoadCharFromFile("resources/block_1_left.txt")); //BRICK_1_LEFT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/block_1_right.txt")); //BRICK_1_RIGHT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_ball_left.txt")); //UI_BALL_LEFT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_ball_right.txt")); //UI_BALL_RIGHT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_bat_left.txt")); //UI_BAT_LEFT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_bat_right.txt")); //UI_BAT_RIGHT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_top.txt")); //UI_TOP
}


std::pair<EC64Color, EC64Color> GetMaterialColor(EMaterial mat)
{
	switch (mat)
	{//sotet - vilagos
	case EM_WOOD: return std::make_pair(EC64Color::brown, EC64Color::orange); break;
	case EM_STONE: return std::make_pair(EC64Color::grey, EC64Color::lightgrey); break;
	case EM_STEEL: return std::make_pair(EC64Color::lightblue, EC64Color::cyan); break;
	case EM_FIRE: return std::make_pair(EC64Color::red, EC64Color::pink); break;
	case EM_GOLD: return std::make_pair(EC64Color::orange, EC64Color::yellow); break;
	case EM_EMERALD: return std::make_pair(EC64Color::green, EC64Color::lightgreen); break;
	default: return std::make_pair(EC64Color::black, EC64Color::black); break;
	}
}

void SBat::Bounce(SBall& Ball)
{

}


void SBall::Bounce()
{
	m_nHp -= 1;
	if (m_nHp == 0)
	{
		m_nVelocity = 0;
		m_eMaterial = EM_NONE;
	}
}

SC64Char SBrick::GetChar(bool bRight)
{
	SC64Char Char;
	switch (m_eMaterial)
	{
	case EM_WOOD: 
		Char = g_aLoadedChars[bRight ? BRICK_1_RIGHT : BRICK_1_LEFT];
		break;
	case EM_STONE:
		Char = g_aLoadedChars[bRight ? BRICK_1_RIGHT : BRICK_1_LEFT];
		break;
	}
	auto Pair = GetMaterialColor(m_eMaterial);
	Char.cBG = Pair.first;
	Char.cFG = Pair.second;
	return Char;
}

void SBrick::SetMaterial(EMaterial mat)
{
	m_eMaterial = mat;
	switch (mat)
	{
	case EM_NONE: m_nHp = 0; break;
	case EM_WOOD: m_nHp = 1; break;
	case EM_STONE: m_nHp = 2; break;
	}
}

void SBrick::Hit(SBall& Ball)
{
	if (m_nHp > 0)
	{
		m_nHp = std::max(0, m_nHp - Ball.m_nDamage);
	}

	if (m_nHp == 0)
	{
		m_eMaterial = EM_NONE;
		m_bDirty = true;
	}
}

void CBreakoutGame::SetCharacter(int x, int y, int nChar, EC64Color cBG, EC64Color cFG)
{
	SC64Char Char = g_aLoadedChars[nChar];
	Char.cBG = cBG;
	Char.cFG = cFG;
	m_pC64->SetCharacter(x, y, Char);
}


void CBreakoutGame::InitLevel()
{
	m_aBricks.clear();
	for(int y=0; y< LEVEL_HEIGHT; ++y)
	{
		for (int x = 0; x < LEVEL_WIDTH; ++x)
		{
			SBrick Brick;
			Brick.SetMaterial((x + y) % 2 ? EM_NONE : ((x + y) % 4 ? EM_STONE : EM_WOOD));
			m_aBricks.push_back(Brick);
		}
	}

	for (int y = 0; y < LEVEL_HEIGHT; ++y)
	{
		for (int x = 0; x < LEVEL_WIDTH; ++x)
		{
			SBrick& Brick = m_aBricks[y*LEVEL_WIDTH + x];
			SC64Char CharLeft = Brick.GetChar(false);
			SC64Char CharRight = Brick.GetChar(true);
			m_pC64->SetCharacter(x * 2, y+1, CharLeft);
			m_pC64->SetCharacter(x * 2 + 1, y+1, CharRight);

		}
	}

	//ui
	for (int x = 0; x < 40; ++x)
	{
		SetCharacter(x, 0, UI_TOP, EC64Color::darkgrey, EC64Color::white);
	}

	SetCharacter(2, 0, UI_BALL_LEFT, EC64Color::darkgrey, EC64Color::white);
	SetCharacter(3, 0, UI_BALL_RIGHT, EC64Color::darkgrey, EC64Color::white);
	SetCharacter(11, 0, UI_BAT_LEFT, EC64Color::darkgrey, EC64Color::white);
	SetCharacter(12, 0, UI_BAT_RIGHT, EC64Color::darkgrey, EC64Color::white);

}

bool CBreakoutGame::Load()
{
	Time::target_fps = 60;

	LoadChars();

	m_pC64 = new CC64RenderLayer(IntRect(0, 0, 960, 600));
	SDLManager::Instance.AddLayer(m_pC64);

	InitLevel();

	m_pC64->Redraw();

	m_Ball.m_eMaterial = EM_STONE;
	m_Ball.m_nMapHp = 50;
	m_Ball.m_nHp = 50;
	m_Ball.m_nDamage = 2;

	m_Ball.m_vPos = { 100,150 };
	m_Ball.m_vSpeed = { 1,1 };
	m_Ball.m_nVelocity = 1;

	m_Bat.m_eMaterial = EM_WOOD;
	m_Bat.m_nPos = 19 * 8;
	m_Bat.m_nWidth = 32;

	return true;
}

SBrick * CBreakoutGame::GetBrickAtPixel(IntVec pos)
{
	IntVec BrickCoord(pos.x / 16, (pos.y-8) / 8);
	int iIndex = BrickCoord.y * 20 + BrickCoord.x;
	if (iIndex > 0 && iIndex < m_aBricks.size())
	{
		return &m_aBricks[iIndex];
	}
	return nullptr;
}

SC64Char CBreakoutGame::LoadCharFromFile(const std::string& path)
{
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	
	std::string sChars = buffer.str();

	SC64Char OutChar;

	int counter = 0;
	for (unsigned int i = 0; i < sChars.size(); ++i)
	{
		if (sChars[i] == '0' || sChars[i] == '1')
		{
			OutChar.aPixels[counter] = sChars[i] == '1';
			counter++;
			if (counter == 64) break;
		}	
	}
	return OutChar;
}




void CBreakoutGame::Update()
{
	m_dFrametimer += Time::frame;
	if (m_dFrametimer < 0.01666) return;

	UpdateBall();
	UpdateBat();

	for (int y = 0; y < LEVEL_HEIGHT; ++y)
	{
		for (int x = 0; x < LEVEL_WIDTH; ++x)
		{
			SBrick& Brick = m_aBricks[y*LEVEL_WIDTH + x];
			if (Brick.m_bDirty)
			{
				SC64Char CharLeft = Brick.GetChar(false);
				SC64Char CharRight = Brick.GetChar(true);
				m_pC64->SetCharacter(x * 2, y+1, CharLeft);
				m_pC64->SetCharacter(x * 2 + 1, y+1, CharRight);
				Brick.m_bDirty = false;
			}
		}
	}

	//ui update
	//ball hp
	int nBallHpWidth = (38 * m_Ball.m_nHp) / m_Ball.m_nMapHp;
	int nFullX = 0;
	auto Pair = GetMaterialColor(m_Ball.m_eMaterial);
	for (int i = 0; i < 5; ++i)
	{
		SC64Char Char;
		for (int x = 0; x < 8; ++x)
		{
			for (int y = 0; y < 8; ++y)
			{
				Char.aPixels[y * 8 + x] = nFullX < nBallHpWidth && y != 7 && y != 0 && nFullX != 0 && nFullX != 39;
			}
			nFullX++;
		}
		if (m_Ball.m_eMaterial != EM_NONE)
		{
			Char.cBG = Pair.first;
			Char.cFG = Pair.second;
		}
		else
		{
			Char.cBG = EC64Color::darkgrey;
			Char.cFG = EC64Color::white;
		}
		m_pC64->SetCharacter(4+i, 0, Char);
	}

	m_pC64->Redraw();

	m_dFrametimer = 0;

	iTest++;

}


void CBreakoutGame::UpdateBall()
{
	IntVec vOldPos = m_Ball.m_vPos;
	for (int it = 0; it < m_Ball.m_nVelocity; ++it)
	{
		m_Ball.m_vPos += m_Ball.m_vSpeed;
		IntVec m_vBallEnd = m_Ball.m_vPos + IntVec(4, 4);
		if (m_Ball.m_vSpeed.x == 1)
		{
			if (m_vBallEnd.x == 40 * 8 - 1)
			{
				m_Ball.m_vSpeed.x = -1;
				m_Ball.Bounce();
			}
			else
			{
				auto pBrick = GetBrickAtPixel(m_Ball.m_vPos + IntVec(4,1) );
				if (pBrick && pBrick->m_eMaterial != EM_NONE)
				{
					pBrick->Hit(m_Ball);
					m_Ball.m_vSpeed.x = -1;
					m_Ball.Bounce();
				}
			}
		}
		else
		{
			if (m_Ball.m_vPos.x == 0)
			{
				m_Ball.m_vSpeed.x = 1;
				m_Ball.Bounce();
			}
			else
			{
				auto pBrick = GetBrickAtPixel(m_Ball.m_vPos + IntVec(-1, 2));
				if (pBrick && pBrick->m_eMaterial != EM_NONE)
				{
					pBrick->Hit(m_Ball);
					m_Ball.m_vSpeed.x = 1;
					m_Ball.Bounce();
				}
			}
		}

		if (m_Ball.m_vSpeed.y == 1)
		{
			//itt kell majd a lezuhanast megoldani
#ifdef NO_FAIL
			if (m_vBallEnd.y == 25 * 8 - 1)
			{
				m_Ball.m_vSpeed.y = -1;
				m_Ball.Bounce();
			}
			else
#endif
				//bat bounce
			if (m_vBallEnd.y == 24 * 8 - 1)
			{
				if (m_vBallEnd.x >= m_Bat.m_nPos && m_vBallEnd.x <= m_Bat.m_nPos + m_Bat.m_nWidth)
				{
					m_Ball.m_vSpeed.y = -1;
					m_Ball.Bounce();
					m_Bat.Bounce(m_Ball);
				}
			}
			else
			{
				auto pBrick = GetBrickAtPixel(m_Ball.m_vPos + IntVec(2, 4));
				if (pBrick && pBrick->m_eMaterial != EM_NONE)
				{
					pBrick->Hit(m_Ball);
					m_Ball.m_vSpeed.y = -1;
					m_Ball.Bounce();
				}
			}
		}
		else
		{
			if (m_Ball.m_vPos.y == 8) //felso sav miatt 8
			{
				m_Ball.m_vSpeed.y = 1;
				m_Ball.Bounce();
			}
			else
			{
				auto pBrick = GetBrickAtPixel(m_Ball.m_vPos + IntVec(1, -1));
				if (pBrick && pBrick->m_eMaterial != EM_NONE)
				{
					pBrick->Hit(m_Ball);
					m_Ball.m_vSpeed.y = 1;
					m_Ball.Bounce();
				}
			}
		}
	}

	if (m_Ball.m_vPos.y <= 0)
	{
		m_Ball.m_eMaterial = EM_NONE;
		m_Ball.m_nVelocity = 0;
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
			m_pC64->SetPixel(m_Ball.m_vPos.x + x, m_Ball.m_vPos.y + y, true);

			auto Pair = GetMaterialColor(m_Ball.m_eMaterial);
			m_pC64->SetPixelColor(m_Ball.m_vPos.x + x, m_Ball.m_vPos.y + y, EC64Color::black, Pair.second);
			
		}
	}
	
}

void CBreakoutGame::UpdateBat()
{
	int nOldPos = m_Bat.m_nPos;
	if (Input::GetKey(KEY_LEFT).active)
	{
		m_Bat.m_nPos = std::max(0, m_Bat.m_nPos - 3);
	}
	if (Input::GetKey(KEY_RIGHT).active)
	{
		m_Bat.m_nPos = std::min( 320 - m_Bat.m_nWidth, m_Bat.m_nPos + 3);
	}

	for (int x = 0; x < m_Bat.m_nWidth; ++x)
	{
		for (int y = 0; y < 4; ++y)
		{
			m_pC64->SetPixel(nOldPos + x, 192 + y, false);
		}
	}

	for (int x = 0; x < m_Bat.m_nWidth; ++x)
	{
		for (int y = 0; y < 4; ++y)
		{
			if (x == 0 && y == 0) continue;
			if (x == 0 && y == 3) continue;
			if (x == m_Bat.m_nWidth-1 && y == 0) continue;
			if (x == m_Bat.m_nWidth-1 && y == 3) continue;
			m_pC64->SetPixel(m_Bat.m_nPos + x, 192 + y, true);

			auto Pair = GetMaterialColor(m_Bat.m_eMaterial);
			m_pC64->SetPixelColor(m_Bat.m_nPos + x, 192 + y, EC64Color::black, Pair.second);

		}
	}

}


void CBreakoutGame::WriteText(const std::string & str, int x, int y)
{
	for (unsigned int i = 0; i < str.size(); ++i)
	{
		if (x + i > 39) return;
		SC64Char Char = m_pC64->GetCharacter(x + i, y);
		Char.SetCharacter(str[i], false);
		m_pC64->SetCharacter(x + i, y, Char);
	}
}