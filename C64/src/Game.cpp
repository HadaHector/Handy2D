#include "Game.h"
#include "SDLManager.h"
#include <algorithm>
#include "Input.h"
#include "Breakout.h"
#include "Audio.h"

extern std::vector<SC64Char> g_aLoadedChars;



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

std::string GetMaterialName(EMaterial mat)
{
	switch (mat)
	{//sotet - vilagos
	case EM_WOOD: return "wood";
	case EM_STONE: return "stone";
	case EM_STEEL: return "steel";
	case EM_FIRE: return "ruby";
	case EM_GOLD: return "gold";
	case EM_EMERALD: return "emerald";
	}
	return "";
}

//BALL

void SBall::Bounce()
{
	m_nHp -= 1;
	if (m_nHp == 0)
	{
		m_nVelocity = 0;
		m_bVisible = false;
		CAudio::PlaySound(CSound::GetSound("resources/destroy.wav"));
	}
	m_nBounceCount++;
	if (m_nBounceCount > 15)
	{
		m_nVelocity = std::min(5, m_nVelocity + 1);
		m_nBounceCount = 0;
	}

	CAudio::PlaySound(CSound::GetSound("resources/bounce.wav"));
}

void SBall::SetMaterial(EMaterial eMat)
{
	m_eMaterial = eMat;
	m_nVelocity = 2;
	m_nPrice = 5;
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
		m_nPrice = 10;
		break;
	case EM_STEEL:
		m_nHp = 150;
		m_nDamage = 3;
		m_nMaxSpd = 6;
		m_nPrice = 10;
		break;
	case EM_FIRE:
		m_nHp = 50;
		m_nDamage = 10;
		m_nMaxSpd = 7;
		m_nVelocity = 3;
		m_nPrice = 15;
		break;
	case EM_GOLD:
		m_nHp = 1000;
		m_nDamage = 6;
		m_nMaxSpd = 6;
		m_nVelocity = 1;
		m_nPrice = 15;
		break;
	case EM_EMERALD:
		m_nHp = 300;
		m_nDamage = 8;
		m_nMaxSpd = 7;
		m_nVelocity = 2;
		m_nPrice = 10;
		break;
	}
	m_nMapHp = m_nHp;
}

// BAT

void SBat::SetMaterial(EMaterial eMat)
{
	m_eMaterial = eMat;
	switch (eMat)
	{
	case EM_WOOD:
		m_nHp = 50;
		m_nPrice = 10;
		break;
	case EM_STONE:
		m_nHp = 100;
		m_nPrice = 10;
		break;
	case EM_STEEL:
		m_nHp = 150;
		m_nPrice = 15;
		break;
	case EM_GOLD:
		m_nHp = 300;
		m_nPrice = 15;
		break;
	case EM_EMERALD:
		m_nHp = 1000;
		m_nPrice = 20;
		break;
	}

	m_nMapHp = m_nHp;
}

// BRICK

SC64Char SBrick::GetChar(bool bRight)
{
	SC64Char Char;
	int iChar = -1;

	bool bInvisible = false;

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
		iChar = BRICK_1_LEFT;
		if (m_nHp == 2)
		{
			bInvisible = true;
		}
		break;
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
		Char = g_aLoadedChars[bRight ? iChar + 1 : iChar];
	}

	auto Pair = GetMaterialColor(m_eMaterial);
	Char.cBG = Pair.first;
	Char.cFG = Pair.second;

	if (bInvisible)
	{
		Char.cBG = Char.cFG = EC64Color::black;
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
	case EM_STONE: m_nHp = 2; break;
	case EM_STEEL: m_nHp = 3; break;
	case EM_FIRE: m_nHp = 2; break;
	case EM_GOLD: m_nHp = 4; break;
	case EM_EMERALD: m_nHp = 8; break;
	}
}

// GAME

CGame::CGame()
{
}


CGame::~CGame()
{
}

void CGame::Activate()
{
	CBreakoutGame* pGame = CBreakoutGame::GetInstance();
	pGame->ClearScreen(EC64Color::black, EC64Color::white);

	StartLevel();
}

CC64RenderLayer* CGame::GetRenderer()
{
	return CBreakoutGame::GetInstance()->m_pC64;
}


void CGame::HitBrick(SBrick& Brick)
{
	if (Brick.m_nHp > 0)
	{
		if (Brick.m_eMaterial == EM_FIRE && Brick.m_nHp == 2)
		{
			Brick.m_nHp--; //a megjelenites mindig 1
		}
		else
		{
			Brick.m_nHp = std::max(0, Brick.m_nHp - m_Ball.m_nDamage);
		}
	}

	if (Brick.m_nHp == 0)
	{
		m_aCollectedMaterials[Brick.m_eMaterial]++;
		Brick.m_eMaterial = EM_NONE;
		
		//nezzuk meg nyertunk-e
		bool bWin = true;
		for (unsigned int i = 0; i < m_aBricks.size(); ++i)
		{
			if (m_aBricks[i].m_eMaterial != EM_NONE)
			{
				bWin = false;
				break;
			}
		}
		if (bWin)
		{
			auto pGame = CBreakoutGame::GetInstance();
			pGame->WriteText("level completed!", 12, 12, EC64Color::black, EC64Color::white);
			CAudio::PlaySound(CSound::GetSound("resources/win.wav"));

			SGameSave& Save = pGame->GetSave();
			for (int i = 0; i < 6; ++i)
			{
				Save.m_aMaterials[i] = m_aCollectedMaterials[EM_WOOD + i];
				Save.m_aBalls[i] = m_aBalls[EM_WOOD + i];
				Save.m_aBats[i] = m_aBats[EM_WOOD + i];
			}
			Save.m_nHighestLevel = std::max((int) Save.m_nHighestLevel, pGame->m_nLevel);
			pGame->Save();

			m_bGameOver = true;

		}
	}

	Brick.m_bDirty = true;
}



void CGame::InitLevel()
{
	m_aBricks.clear();
	
	CBreakoutGame* pGame = CBreakoutGame::GetInstance();

	CLevel Level = pGame->m_aLevels[pGame->m_nLevel];

	for (int y = 0; y < LEVEL_HEIGHT; ++y)
	{
		for (int x = 0; x < LEVEL_WIDTH; ++x)
		{
			SBrick Brick;
			Brick.SetMaterial((EMaterial) Level.m_aBricks[y*LEVEL_WIDTH+x]);
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
			GetRenderer()->SetCharacter(x * 2, y + 1, CharLeft);
			GetRenderer()->SetCharacter(x * 2 + 1, y + 1, CharRight);

		}
	}

	SC64Char Char;
	Char.cBG = EC64Color::darkgrey;
	Char.cFG = EC64Color::white;
	for (int x = 0; x < 40; ++x)
	{
		GetRenderer()->SetCharacter(x, 0, Char);
	}

	pGame->SetCharacter(2, 0, UI_BALL_LEFT, EC64Color::darkgrey, EC64Color::white);
	pGame->SetCharacter(3, 0, UI_BALL_RIGHT, EC64Color::darkgrey, EC64Color::white);
	pGame->SetCharacter(11, 0, UI_BAT_LEFT, EC64Color::darkgrey, EC64Color::white);
	pGame->SetCharacter(12, 0, UI_BAT_RIGHT, EC64Color::darkgrey, EC64Color::white);

}


void CGame::StartLevel()
{
	InitLevel();

	GetRenderer()->Redraw();

	auto pGame = CBreakoutGame::GetInstance();
	SGameSave& Save = pGame->GetSave();

	int nBallType = -1;
	int nBatType = -1;
	for (int i = 0; i < 6; ++i)
	{
		m_aCollectedMaterials[EM_WOOD + i] = Save.m_aMaterials[i];
		m_aBalls[EM_WOOD + i] = Save.m_aBalls[i];
		m_aBats[EM_WOOD + i] = Save.m_aBats[i];
		if (nBallType == -1 && Save.m_aBalls[i] > 0)
		{
			nBallType = i;
		}
		if (nBatType == -1 && Save.m_aBats[i] > 0)
		{
			nBatType = i;
		}
	}

	//adunk egy freet, hogy minden fele keppen lehessen jatszani
	if (nBallType == -1)
	{
		m_aBalls[EM_WOOD]++;
	}
	if (nBatType == -1)
	{
		m_aBats[EM_WOOD]++;
	}

	m_Ball.SetMaterial(EM_NONE);
	m_Bat.SetMaterial(EM_NONE);
	NextBallMaterial();
	NextBatMaterial();

	m_Bat.m_nPos = 19 * 8;
	m_Bat.m_nWidth = 32;

	m_bBallOnBat = true;
	m_bTempBall = true;
	m_bTempBat = true;
	m_dRespawnTime = -1;
	m_bGameOver = false;



}

SBrick* CGame::GetBrickAtPixel(IntVec pos)
{
	IntVec BrickCoord(pos.x / 16, (pos.y - 8) / 8);
	unsigned int iIndex = BrickCoord.y * 20 + BrickCoord.x;
	if (iIndex > 0 && iIndex < m_aBricks.size())
	{
		return &m_aBricks[iIndex];
	}
	return nullptr;
}



void CGame::Update()
{
	if (m_bTempBall && Input::GetKey(KEY_Q).pressed)
	{
		NextBallMaterial();
	}

	if (m_bTempBat && Input::GetKey(KEY_A).pressed)
	{
		NextBatMaterial();
	}

	if (m_bGameOver && Input::GetKey(KEY_SPACE).pressed)
	{
		CBreakoutGame::GetInstance()->SwitchState(Shop);
		return;
	}

	if (Input::GetKey(KEY_ESCAPE).pressed)
	{
		auto pGame = CBreakoutGame::GetInstance();
		SGameSave& Save = pGame->GetSave();
		for (int i = 0; i < 6; ++i)
		{
			Save.m_aMaterials[i] = m_aCollectedMaterials[EM_WOOD + i];
			Save.m_aBalls[i] = m_aBalls[EM_WOOD + i];
			Save.m_aBats[i] = m_aBats[EM_WOOD + i];
		}
		pGame->Save();
		CBreakoutGame::GetInstance()->SwitchState(Shop);
		return;
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
				m_dRespawnTime = Time::full + 0.75;
				bOk = true;
				break;
			}
		}

		if (!m_Bat.m_bVisible) bOk = false;

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
			if (!m_bGameOver)
			{
				auto pGame = CBreakoutGame::GetInstance();
				pGame->WriteText("game over", 15, 12, EC64Color::black, EC64Color::white);
				CAudio::PlaySound(CSound::GetSound("resources/loose.wav"));

				SGameSave& Save = pGame->GetSave();
				for (int i = 0; i < 6; ++i)
				{
					Save.m_aMaterials[i] = m_aCollectedMaterials[EM_WOOD + i];
					Save.m_aBalls[i] = m_aBalls[EM_WOOD + i];
					Save.m_aBats[i] = m_aBats[EM_WOOD + i];
				}
				pGame->Save();
			}
			
			m_bGameOver = true;
			
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
			m_Bat.SetMaterial(m_Bat.m_eMaterial);
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
				GetRenderer()->SetCharacter(x * 2, y + 1, CharLeft);
				GetRenderer()->SetCharacter(x * 2 + 1, y + 1, CharRight);
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
	CBreakoutGame::GetInstance()->DrawHpBar(4, (float)m_Ball.m_nHp / m_Ball.m_nMapHp, Pair.first, Pair.second);


	//bat hp

	Pair = GetMaterialColor(m_Bat.m_eMaterial);
	if (m_Bat.m_eMaterial == EM_NONE)
	{
		Pair.first = EC64Color::darkgrey;
		Pair.second = EC64Color::white;
	}
	CBreakoutGame::GetInstance()->DrawHpBar(13, (float)m_Bat.m_nHp / m_Bat.m_nMapHp, Pair.first, Pair.second);


	CBreakoutGame::GetInstance()->WriteNumber(1, 0, m_aBalls[m_Ball.m_eMaterial], EC64Color::darkgrey, EC64Color::white);
	CBreakoutGame::GetInstance()->WriteNumber(10, 0, m_aBats[m_Bat.m_eMaterial], EC64Color::darkgrey, EC64Color::white);

	for (int i = 0; i < 6; ++i)
	{
		EMaterial eMat = (EMaterial)(EM_WOOD + i);
		auto Pair = GetMaterialColor(eMat);
		CBreakoutGame::GetInstance()->WriteDoubleNumber(20 + 3 * i, 0, m_aCollectedMaterials[eMat], Pair.first, Pair.second);
	}


	GetRenderer()->Redraw();

	m_dFrametimer = 0;

	iFrameCounter++;

}


void CGame::UpdateBall()
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

	if (!m_Ball.m_bVisible) nSpd = 0;

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
				auto pBrick = GetBrickAtPixel(m_Ball.m_vPos + IntVec(4, 1));
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
				if (m_Bat.m_bVisible && m_vBallEnd.y == 24 * 8 - 1)
				{
					if (m_vBallEnd.x >= m_Bat.m_nPos && m_Ball.m_vPos.x <= m_Bat.m_nPos + m_Bat.m_nWidth)
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

	if (m_Ball.m_vPos.y >= 25 * 8 && m_Ball.m_bVisible)
	{
		m_Ball.m_bVisible = false;
		m_Ball.m_nVelocity = 0;
		CAudio::PlaySound(CSound::GetSound("resources/destroy.wav"));
	}

	auto pRenderer = GetRenderer();

	for (int x = 0; x < 4; ++x)
	{
		for (int y = 0; y < 4; ++y)
		{
			pRenderer->SetPixel(vOldPos.x + x, vOldPos.y + y, false);
		}
	}

	if (m_bTempBall && Time::full * 2 - floor(Time::full * 2) < 0.5f) return;
	if (m_bTempBall && m_bTempBat) return;
	if (!m_Ball.m_bVisible) return;
	if (m_bGameOver) return;

	for (int x = 0; x < 4; ++x)
	{
		for (int y = 0; y < 4; ++y)
		{
			if (x == 0 && y == 0) continue;
			if (x == 0 && y == 3) continue;
			if (x == 3 && y == 0) continue;
			if (x == 3 && y == 3) continue;
			pRenderer->SetPixel(m_Ball.m_vPos.x + x, m_Ball.m_vPos.y + y, true);

			auto Pair = GetMaterialColor(m_Ball.m_eMaterial);
			pRenderer->SetPixelColor(m_Ball.m_vPos.x + x, m_Ball.m_vPos.y + y, EC64Color::black, Pair.second);

		}
	}

}

void CGame::UpdateBat()
{
	int nOldPos = m_Bat.m_nPos;
	if (Input::GetKey(KEY_LEFT).active)
	{
		m_Bat.m_nPos = std::max(0, m_Bat.m_nPos - 3);
	}
	if (Input::GetKey(KEY_RIGHT).active)
	{
		m_Bat.m_nPos = std::min(320 - m_Bat.m_nWidth, m_Bat.m_nPos + 3);
	}

	auto pRenderer = GetRenderer();

	if (m_bBallOnBat)
	{
		for (int x = 0; x < 4; ++x)
		{
			for (int y = 0; y < 4; ++y)
			{
				pRenderer->SetPixel(m_Ball.m_vPos.x + x, m_Ball.m_vPos.y + y, false);
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
			pRenderer->SetPixel(nOldPos + x, 192 + y, false);
		}
	}

	if (m_bTempBat && Time::full * 2 - floor(Time::full * 2) < 0.5f) return;
	if (!m_Bat.m_bVisible) return;

	for (int x = 0; x < m_Bat.m_nWidth; ++x)
	{
		for (int y = 0; y < 4; ++y)
		{
			if (x == 0 && y == 0) continue;
			if (x == 0 && y == 3) continue;
			if (x == m_Bat.m_nWidth - 1 && y == 0) continue;
			if (x == m_Bat.m_nWidth - 1 && y == 3) continue;
			pRenderer->SetPixel(m_Bat.m_nPos + x, 192 + y, true);

			auto Pair = GetMaterialColor(m_Bat.m_eMaterial);
			pRenderer->SetPixelColor(m_Bat.m_nPos + x, 192 + y, EC64Color::black, Pair.second);

		}
	}

}



void CGame::BounceOnBat()
{
	if (m_bTempBat)
	{
		m_bTempBat = false;
		m_aBats[m_Bat.m_eMaterial] --;
		m_Bat.SetMaterial(m_Bat.m_eMaterial);
	}

	m_Bat.m_nHp -= m_Ball.m_nDamage;
	if (m_Bat.m_nHp <= 0)
	{
		CAudio::PlaySound(CSound::GetSound("resources/destroy.wav"));

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

void CGame::NextBallMaterial()
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


void CGame::NextBatMaterial()
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