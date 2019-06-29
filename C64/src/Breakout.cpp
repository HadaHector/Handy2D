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
#define UI_0 6
#define UI_1 7
#define UI_2 8
#define UI_3 9
#define UI_4 10
#define UI_5 11
#define UI_6 12
#define UI_7 13
#define UI_8 14
#define UI_9 15
#define BRICK_2_LEFT 16
#define BRICK_2_RIGHT 17
#define BRICK_3_LEFT 18
#define BRICK_3_RIGHT 19 
#define BRICK_4_LEFT 20 
#define BRICK_4_RIGHT 21

void CBreakoutGame::LoadChars()
{
	g_aLoadedChars.push_back(LoadCharFromFile("resources/block_1_left.txt")); //BRICK_1_LEFT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/block_1_right.txt")); //BRICK_1_RIGHT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_ball_left.txt")); //UI_BALL_LEFT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_ball_right.txt")); //UI_BALL_RIGHT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_bat_left.txt")); //UI_BAT_LEFT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_bat_right.txt")); //UI_BAT_RIGHT
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_0.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_1.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_2.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_3.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_4.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_5.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_6.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_7.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_8.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/ui_9.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/block_2_left.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/block_2_right.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/block_3_left.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/block_3_right.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/block_4_left.txt"));
	g_aLoadedChars.push_back(LoadCharFromFile("resources/block_4_right.txt"));
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


void SBall::Bounce()
{
	m_nHp -= 1;
	if (m_nHp == 0)
	{
		m_nVelocity = 0;
		m_eMaterial = EM_NONE;
	}
	m_nBounceCount++;
	if (m_nBounceCount > 20)
	{
		m_nVelocity = std::min(5, m_nVelocity + 1);
		m_nBounceCount = 0;
	}
}

void SBall::SetMaterial(EMaterial eMat)
{
	m_eMaterial = eMat;
	switch (eMat)
	{
	case EM_WOOD:
		m_nHp = 50;
		m_nDamage = 1;
		m_nMaxSpd = 4;
		break;
	case EM_STONE:
		m_nHp = 100;
		m_nDamage = 2;
		m_nMaxSpd = 5;
		break;
	case EM_STEEL:
		m_nHp = 150;
		m_nDamage = 3;
		m_nMaxSpd = 6;
		break;
	case EM_FIRE:
		m_nHp = 50;
		m_nDamage = 10;
		m_nMaxSpd = 7;
		m_nVelocity = 3;
		break;
	case EM_GOLD:
		m_nHp = 1000;
		m_nDamage = 6;
		m_nMaxSpd = 6;
		m_nVelocity = 1;
		break;
	case EM_EMERALD:
		m_nHp = 300;
		m_nDamage = 8;
		m_nMaxSpd = 7;
		m_nVelocity = 2;
		break;
	}
	m_nMapHp = m_nHp;
}


void SBat::SetMaterial(EMaterial eMat)
{
	m_eMaterial = eMat;
	switch (eMat)
	{
	case EM_WOOD:
		m_nHp = 50;
		break;
	case EM_STONE:
		m_nHp = 100;
		break;
	case EM_STEEL:
		m_nHp = 150;
		break;
	case EM_GOLD:
		m_nHp = 300;
		break;
	case EM_EMERALD:
		m_nHp = 1000;
		break;
	}

	m_nMapHp = m_nHp;
}

SC64Char SBrick::GetChar(bool bRight)
{
	SC64Char Char;
	int iChar = -1;
	switch (m_eMaterial)
	{
	case EM_WOOD: 
		iChar = BRICK_1_LEFT;
		break;
	case EM_STONE:
		switch (m_nHp)
		{
		case 1: iChar = BRICK_1_LEFT; break;
		case 2: iChar = BRICK_2_LEFT; break;
		}
		break;
	case EM_STEEL:
		switch (m_nHp)
		{
		case 1: iChar = BRICK_1_LEFT; break;
		case 2: iChar = BRICK_2_LEFT; break;
		case 3: iChar = BRICK_3_LEFT; break;
		}
		break;
	case EM_FIRE:
		iChar = BRICK_4_LEFT; break;
	case EM_GOLD:
		switch (m_nHp)
		{
		case 1: iChar = BRICK_1_LEFT; break;
		case 2: iChar = BRICK_2_LEFT; break;
		case 3: iChar = BRICK_3_LEFT; break;
		case 4: iChar = BRICK_4_LEFT; break;
		}
		break;
	case EM_EMERALD:
		switch (m_nHp)
		{
		case 1:
		case 2:	iChar = BRICK_1_LEFT; break;
		case 3:
		case 4:	iChar = BRICK_2_LEFT; break;
		case 5:
		case 6: iChar = BRICK_3_LEFT; break;
		case 7:
		case 8: iChar = BRICK_4_LEFT; break;
		}
		break;
	}
	
	if (iChar != -1)
	{
		Char = g_aLoadedChars[bRight ? iChar+1 : iChar];
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
	case EM_STEEL: m_nHp = 3; break;
	case EM_FIRE: m_nHp = 1; break;
	case EM_GOLD: m_nHp = 4; break;
	case EM_EMERALD: m_nHp = 8; break;
	}
}


void CBreakoutGame::HitBrick(SBrick& Brick)
{
	if (Brick.m_nHp > 0)
	{
		Brick.m_nHp = std::max(0, Brick.m_nHp - m_Ball.m_nDamage);
	}

	if (Brick.m_nHp == 0)
	{
		m_aCollectedMaterials[Brick.m_eMaterial]++;
		Brick.m_eMaterial = EM_NONE;
	}

	Brick.m_bDirty = true;
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

	SC64Char Char;
	Char.cBG = EC64Color::darkgrey;
	Char.cFG = EC64Color::white;
	for (int x = 0; x < 40; ++x)
	{
		m_pC64->SetCharacter(x, 0, Char);
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

	m_Ball.SetMaterial(EM_WOOD);
	m_Bat.SetMaterial(EM_WOOD);

	m_Bat.m_nPos = 19 * 8;
	m_Bat.m_nWidth = 32;

	m_bBallOnBat = true;
	m_bTempBall = true;
	m_bTempBat = true;
	m_dRespawnTime = -1;
	
	m_aBalls[EM_WOOD] = 3;
	m_aBalls[EM_STONE] = 1;
	m_aBalls[EM_STEEL] = 1;
	m_aBalls[EM_FIRE] = 1;
	m_aBalls[EM_GOLD] = 1;
	m_aBalls[EM_EMERALD] = 1;

	m_aBats[EM_WOOD] = 3;
	m_aBats[EM_STONE] = 1;
	m_aBats[EM_STEEL] = 1;
	m_aBats[EM_GOLD] = 1;
	m_aBats[EM_EMERALD] = 1;
	
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
	if (m_bTempBall && Input::GetKey(KEY_Q).pressed)
	{
		NextBallMaterial();
	}

	if (m_bTempBat && Input::GetKey(KEY_A).pressed)
	{
		NextBatMaterial();
	}
	

	m_dFrametimer += Time::frame;
	if (m_dFrametimer < 0.01666) return;

	//respawn update
	if (!m_bBallOnBat && !m_Ball.m_bVisible && m_dRespawnTime == -1)
	{
		bool bOk = false;

		//nezzuk meg maradt e labdaja
		for (int i = EM_WOOD; i <= EM_EMERALD; ++i)
		{
			if (m_aBalls[(EMaterial)i] > 0)
			{
				m_dRespawnTime = Time::full + 1;
				bOk = true;
				break;
			}
		}

		if (bOk)
		{
			if (m_aBalls[m_Ball.m_eMaterial] <= 0)
			{
				NextBallMaterial();
			}
		}
		else
		{
			//GAME OVER
			WriteText("game over", 15, 12);
		}
	}

	if (m_dRespawnTime != -1 && m_dRespawnTime < Time::full)
	{
		m_bBallOnBat = true;
		m_Ball.m_nHp = 50;
		m_dRespawnTime = -1;
		m_bTempBall = true;
		m_Ball.m_vPos.y = 192 - 4;
		m_Ball.m_bVisible = true;
	}

	UpdateBat();
	UpdateBall();

	if (Input::GetKey(KEY_SPACE).pressed)
	{
		if (m_bTempBat)
		{
			m_aBats[m_Bat.m_eMaterial] --;
			m_bTempBat = false;
		}
		else if (m_bTempBall)
		{
			m_aBalls[m_Ball.m_eMaterial] --;
			m_bTempBall = false;
			m_bBallOnBat = false;
			m_Ball.SetMaterial(m_Ball.m_eMaterial);
		}
	}

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


	//ui
	//ball hp

	auto Pair = GetMaterialColor(m_Ball.m_eMaterial);
	if (m_Ball.m_eMaterial == EM_NONE)
	{
		Pair.first = EC64Color::darkgrey;
		Pair.second = EC64Color::white;
	}
	DrawHpBar(4, (float)m_Ball.m_nHp / m_Ball.m_nMapHp, Pair.first, Pair.second);


	//bat hp

	Pair = GetMaterialColor(m_Bat.m_eMaterial);
	if (m_Bat.m_eMaterial == EM_NONE)
	{
		Pair.first = EC64Color::darkgrey;
		Pair.second = EC64Color::white;
	}
	DrawHpBar(13, (float)m_Bat.m_nHp / m_Bat.m_nMapHp, Pair.first, Pair.second);


	WriteTopText(1, m_aBalls[m_Ball.m_eMaterial], EC64Color::darkgrey, EC64Color::white);
	WriteTopText(10, m_aBats[m_Bat.m_eMaterial], EC64Color::darkgrey, EC64Color::white);

	for (int i = 0; i < 6; ++i)
	{
		EMaterial eMat = (EMaterial)(EM_WOOD + i);
		auto Pair = GetMaterialColor(eMat);
		WriteDoubleTopText(20+2*i, m_aCollectedMaterials[eMat], Pair.first, Pair.second);
	}


	m_pC64->Redraw();

	m_dFrametimer = 0;

	iFrameCounter++;

}


void CBreakoutGame::UpdateBall()
{
	IntVec vOldPos = m_Ball.m_vPos;
	int nSpd = 1;
	if (m_Ball.m_nVelocity > 1)
	{
		if (iFrameCounter % (8 - m_Ball.m_nVelocity) == 0)
		{
			nSpd++;
		}
	}

	for (int it = 0; it < nSpd; ++it)
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
					HitBrick(*pBrick);
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
					HitBrick(*pBrick);
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
					BounceOnBat();
				}
			}
			else
			{
				auto pBrick = GetBrickAtPixel(m_Ball.m_vPos + IntVec(2, 4));
				if (pBrick && pBrick->m_eMaterial != EM_NONE)
				{
					HitBrick(*pBrick);
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
					HitBrick(*pBrick);
					m_Ball.m_vSpeed.y = 1;
					m_Ball.Bounce();
				}
			}
		}
	}

	if (m_Ball.m_vPos.y >= 25*8)
	{
		m_Ball.m_bVisible = false;
		m_Ball.m_nVelocity = 0;
	}
	
	for (int x = 0; x < 4; ++x)
	{
		for (int y = 0; y < 4; ++y)
		{
			m_pC64->SetPixel(vOldPos.x + x, vOldPos.y + y, false);
		}
	}

	if (m_bTempBall && Time::full * 2 - floor(Time::full * 2) < 0.5f) return;
	if (m_bTempBall && m_bTempBat) return;
	if (!m_Ball.m_bVisible) return;
	
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

	if (m_bBallOnBat)
	{
		for (int x = 0; x < 4; ++x)
		{
			for (int y = 0; y < 4; ++y)
			{
				m_pC64->SetPixel(m_Ball.m_vPos.x + x, m_Ball.m_vPos.y + y, false);
			}
		}

		m_Ball.m_nVelocity = 0;
		m_Ball.m_vSpeed = { 1,-1 };
		m_Ball.m_vPos = { m_Bat.m_nPos + m_Bat.m_nWidth / 2,192 - 4 };
	}

			

	for (int x = 0; x < m_Bat.m_nWidth; ++x)
	{
		for (int y = 0; y < 4; ++y)
		{
			m_pC64->SetPixel(nOldPos + x, 192 + y, false);
		}
	}

	if (m_bTempBat && Time::full*2 - floor(Time::full*2) < 0.5f) return;

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
		Char.cBG = EC64Color::white;
		Char.cFG = EC64Color::black;
		m_pC64->SetCharacter(x + i, y, Char);
	}
}

 

void CBreakoutGame::BounceOnBat()
{
	m_bTempBat = false;
	m_Bat.m_nHp -= m_Ball.m_nDamage;
	if (m_Bat.m_nHp <= 0)
	{
		bool bOk = false;
		for (int i = EM_WOOD; i <= EM_EMERALD; ++i)
		{
			if (m_aBats[(EMaterial)i] > 0)
			{
				bOk = true;
				break;
			}
		}

		if (!bOk)
		{
			m_Bat.m_bVisible = false;
		}
		else
		{
			if (m_aBats[m_Bat.m_eMaterial] <= 0)
			{
				NextBatMaterial();
			}
			m_bTempBat = true;
		}

		
	}
}

void CBreakoutGame::NextBallMaterial()
{
	EMaterial eMat = m_Ball.m_eMaterial;
	do
	{
		eMat = (EMaterial)((eMat + 1) % EM_LAST);
		if (eMat == EM_NONE) eMat = EM_WOOD;
		if (m_aBalls[eMat] > 0) break;
		if (eMat == m_Ball.m_eMaterial) break; //korbeertunk, de igazabol mar nincs semmibol
	} while (true);
	m_Ball.m_eMaterial = eMat;
}


void CBreakoutGame::NextBatMaterial()
{
	EMaterial eMat = m_Bat.m_eMaterial;
	do
	{
		eMat = (EMaterial)((eMat + 1) % EM_LAST);
		if (eMat == EM_NONE) eMat = EM_WOOD;
		if (m_aBats[eMat] > 0) break;
		if (eMat == m_Bat.m_eMaterial) break; //korbeertunk, de igazabol mar nincs semmibol
	} while (true);
	m_Bat.m_eMaterial = eMat;
}


void CBreakoutGame::WriteTopText(int x, int val, EC64Color cBG, EC64Color cFG)
{
	val = std::min(val, 99);

	SC64Char Char;
	Char.cBG = cBG;
	Char.cFG = cFG;
	if (val > 10)
	{
		for (int x = 0; x < 4; ++x)
		{
			for (int y = 0; y <= 8; ++y)
			{
				Char.aPixels[y*8+x] = g_aLoadedChars[UI_0 + val / 10].aPixels[y * 8 + x];
			}
		}
	}

	for (int x = 0; x < 4; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			Char.aPixels[y * 8 + x + 4] = g_aLoadedChars[UI_0 + val % 10].aPixels[y * 8 + x + 4];
		}
	}

	m_pC64->SetCharacter(x, 0, Char);
}

void CBreakoutGame::WriteDoubleTopText(int x, int val, EC64Color cBG, EC64Color cFG)
{
	val = std::min(val, 99);

	SC64Char Char1;
	Char1.cBG = cBG;
	Char1.cFG = cFG;

	for (int x = 0; x < 4; ++x)
	{
		for (int y = 0; y <= 8; ++y)
		{
			Char1.aPixels[y * 8 + x] = g_aLoadedChars[UI_0 + val / 10].aPixels[y * 8 + x];
		}
	}

	for (int x = 0; x < 4; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			Char1.aPixels[y * 8 + x + 4] = g_aLoadedChars[UI_0 + val % 10].aPixels[y * 8 + x + 4];
		}
	}

	m_pC64->SetCharacter(x+1, 0, Char1);

	SC64Char Char2;
	Char2.cBG = cBG;
	Char2.cFG = cFG;

	for (int x = 0; x < 4; ++x)
	{
		for (int y = 0; y <= 8; ++y)
		{
			Char2.aPixels[y * 8 + x] = g_aLoadedChars[UI_0 + val / 1000].aPixels[y * 8 + x];
		}
	}

	for (int x = 0; x < 4; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			Char2.aPixels[y * 8 + x + 4] = g_aLoadedChars[UI_0 + val / 100].aPixels[y * 8 + x + 4];
		}
	}

	m_pC64->SetCharacter(x, 0, Char2);
}


void CBreakoutGame::DrawHpBar(int nPos, float fValue, EC64Color cBG, EC64Color cFG)
{
	int nBallHpWidth = (int)(38 * fValue);
	int nFullX = 0;
	for (int i = 0; i < 5; ++i)
	{
		SC64Char Char;
		Char.cBG = cBG;
		Char.cFG = cFG;

		for (int x = 0; x < 8; ++x)
		{
			for (int y = 0; y < 8; ++y)
			{
				Char.aPixels[y * 8 + x] = nFullX < nBallHpWidth && y != 7 && y != 0 && nFullX != 0 && nFullX != 39;
			}
			nFullX++;
		}
		m_pC64->SetCharacter(nPos + i, 0, Char);
	}
}