#include "SDLManager.h"
#include "Breakout.h"
#include "Input.h"

#include <fstream>
#include <sstream>

#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 16


std::vector<SC64Char> g_aLoadedChars;
#define BRICK_1_LEFT 0
#define BRICK_1_RIGHT 1


SC64Char SBrick::GetChar(bool bRight)
{
	SC64Char Char;
	switch (m_eMaterial)
	{
	case EM_NONE:
		Char.cBG = EC64Color::black;
		Char.cFG = EC64Color::white;
		break;
	case EM_WOOD: 
		Char = g_aLoadedChars[bRight ? BRICK_1_RIGHT : BRICK_1_LEFT];
		Char.cBG = EC64Color::brown;
		Char.cFG = EC64Color::orange;
		break;
	}
	return Char;
}

void SBrick::SetMaterial(EMaterial mat)
{
	m_eMaterial = mat;
	switch (mat)
	{
	case EM_NONE: m_nHp = 0; break;
	case EM_WOOD: m_nHp = 1; break;
	}
}



void CBreakoutGame::LoadChars()
{
	g_aLoadedChars.push_back(LoadCharFromFile("resources/block_1_left.txt")); //BRICK_1_LEFT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/block_1_right.txt")); //BRICK_1_RIGHT
}

void CBreakoutGame::InitLevel()
{
	m_aBricks.clear();
	for(int y=0; y< LEVEL_HEIGHT; ++y)
	{
		for (int x = 0; x < LEVEL_WIDTH; ++x)
		{
			SBrick Brick;
			Brick.m_eMaterial = (x + y) % 2 ? EM_NONE : EM_WOOD;
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
			m_pC64->SetCharacter(x * 2, y, CharLeft);
			m_pC64->SetCharacter(x * 2 + 1, y, CharRight);

		}
	}
}

bool CBreakoutGame::Load()
{
	Time::target_fps = 60;

	LoadChars();

	m_pC64 = new CC64RenderLayer(IntRect(0, 0, 1280, 800));
	SDLManager::Instance.AddLayer(m_pC64);

	InitLevel();

	m_vBallPos = { 100,50 };
	m_vBallSpeed = { 1,1 };
	m_nBallVelocity = 5;
	return true;
}

SC64Char CBreakoutGame::LoadCharFromFile(const std::string& path)
{
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	
	std::string sChars = buffer.str();

	SC64Char OutChar;

	int counter = 0;
	for (int i = 0; i < sChars.size(); ++i)
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
	m_pC64->Redraw();
	m_dFrametimer = 0;

	iTest++;

}


void CBreakoutGame::UpdateBall()
{
	IntVec vOldPos = m_vBallPos;
	for (int it = 0; it < m_nBallVelocity; ++it)
	{
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