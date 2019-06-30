#include "Shop.h"
#include "Breakout.h"
#include "Input.h"
#include "Game.h"
#include "Audio.h"

CShop::CShop()
{
}


CShop::~CShop()
{
}

void CShop::Activate()
{
	Refresh();
}

void CShop::Update()
{
	if (Input::GetKey(KEY_SPACE).pressed)
	{
		CBreakoutGame* pGame = CBreakoutGame::GetInstance();

		if (m_nSelected == 0)
		{
			pGame->m_nLevel = m_nSelectedLevel;
			pGame->SwitchState(Game);
		}
		else if (m_nSelected == 1)
		{
			SGameSave& Save = pGame->GetSave();
			if (Save.m_aMaterials[m_nSelectedBall] >= m_nPrice)
			{
				Save.m_aMaterials[m_nSelectedBall] -= m_nPrice;
				Save.m_aBalls[m_nSelectedBall]++;
				pGame->Save();
				Refresh();
				CAudio::PlaySound(CSound::GetSound("resources/buy.wav"));
			}
			else
			{
				CAudio::PlaySound(CSound::GetSound("resources/nobuy.wav"));
			}
		}
		else if (m_nSelected == 2)
		{
			SGameSave& Save = pGame->GetSave();
			if (Save.m_aMaterials[m_nSelectedBat] >= m_nPrice)
			{
				Save.m_aMaterials[m_nSelectedBat] -= m_nPrice;
				Save.m_aBats[m_nSelectedBat]++;
				pGame->Save();
				Refresh();
				CAudio::PlaySound(CSound::GetSound("resources/buy.wav"));
			}
			else
			{
				CAudio::PlaySound(CSound::GetSound("resources/nobuy.wav"));
			}
		}
		else if (m_nSelected == 3)
		{
			pGame->m_bRunning = false;
		}
	}

	if (Input::GetKey(KEY_DOWN).pressed)
	{
		m_nSelected = (m_nSelected + 1) % 4;
		Refresh();
	}
	if (Input::GetKey(KEY_UP).pressed)
	{
		m_nSelected = (m_nSelected + 4 - 1) % 4;
		Refresh();
	}
	if (Input::GetKey(KEY_LEFT).pressed)
	{
		switch (m_nSelected)
		{
		case 0: m_nSelectedLevel = (m_nSelectedLevel + 10 - 1) % 10; break;
		case 1: m_nSelectedBall = (m_nSelectedBall + 6 - 1) % 6; break;
		case 2: m_nSelectedBat = (m_nSelectedBat + 6 - 1) % 6;
			if (m_nSelectedBat == 3) m_nSelectedBat--;
			break;
		}
		Refresh();
	}
	if (Input::GetKey(KEY_RIGHT).pressed)
	{
		switch (m_nSelected)
		{
		case 0: m_nSelectedLevel = (m_nSelectedLevel + 1) % 10; break;
		case 1: m_nSelectedBall = (m_nSelectedBall + 1) % 6; break;
		case 2: m_nSelectedBat = (m_nSelectedBat + 1) % 6; 
			if (m_nSelectedBat == 3) m_nSelectedBat++;
			break;
		}
		Refresh();
	}
}

void CShop::Refresh()
{
	CBreakoutGame* pGame = CBreakoutGame::GetInstance();

	pGame->ClearScreen(EC64Color::grey, EC64Color::white);


	EC64Color cBG = EC64Color::grey;
	EC64Color cFG = EC64Color::white;

	SC64Char Char;
	Char.cBG = EC64Color::darkgrey;
	Char.cFG = EC64Color::white;
	for (int x = 0; x < 18; ++x)
	{
		for (int y = 0; y < 25; ++y)
		{
			pGame->m_pC64->SetCharacter(x, y, Char);
		}
	}

	Char.cBG = EC64Color::lightblue;
	Char.cFG = EC64Color::white;
	for (int x = 0; x < 18; ++x)
	{
		for (int y = 1; y < 6; ++y)
		{
			pGame->m_pC64->SetCharacter(x, y, Char);
		}
	}

	pGame->WriteText("strategy", 2, 2, EC64Color::lightblue, EC64Color::cyan);
	pGame->WriteText("breakout", 2, 4, EC64Color::lightblue, EC64Color::cyan);

	pGame->WriteText("sTART gAME", 2, 9, EC64Color::darkgrey, m_nSelected == 0 ? EC64Color::yellow : EC64Color::white);
	pGame->WriteText("bALL SHOP", 2, 11, EC64Color::darkgrey, m_nSelected == 1 ? EC64Color::yellow : EC64Color::white);
	pGame->WriteText("bAT SHOP", 2, 13, EC64Color::darkgrey, m_nSelected == 2 ? EC64Color::yellow : EC64Color::white);
	pGame->WriteText("qUIT", 2, 15, EC64Color::darkgrey, m_nSelected == 3 ? EC64Color::yellow : EC64Color::white );

	if (m_nSelected == 0) //level select
	{
		pGame->WriteText("level", 24, 11, cBG, cFG);
		pGame->WriteText(std::to_string(m_nSelectedLevel+1), 30, 11, cBG, cFG);
		pGame->WriteText("<", 21, 11, cBG, cFG);
		pGame->WriteText(">", 35, 11, cBG, cFG);

		const int nPosX = 190;
		const int nPosY = 32;
		
		//preview
		CLevel Level = pGame->m_aLevels[m_nSelectedLevel];
		for (int y = 0; y < LEVEL_HEIGHT; ++y)
		{
			for (int x = 0; x < LEVEL_WIDTH; ++x)
			{
				for (int px = 0; px < 4; px++)
				{
					for (int py = 0; py < 2; py++)
					{
						pGame->m_pC64->SetPixel(nPosX + x*4 + px, nPosY + y*2 + py, Level.m_aBricks[y*LEVEL_WIDTH + x] > 0);
						pGame->m_pC64->SetPixelColor(nPosX + x*4 +px, nPosY + y*2 + py, EC64Color::black, EC64Color::orange);
					}
				}
			}
		}
		
		SC64Char Char;
		Char.cBG = EC64Color::black;
		Char.cFG = EC64Color::orange;
		for (int x = 23; x < 34; ++x)
		{
			pGame->m_pC64->SetCharacter(x, 8, Char);
			pGame->m_pC64->SetCharacter(x, 9, Char);
		}

		SGameSave& Save = pGame->GetSave();
		m_bLevelLocked = Save.m_nHighestLevel + 1 > m_nSelectedLevel;
		if (Save.m_nHighestLevel + 1 < m_nSelectedLevel)
		{
			pGame->WriteText("locked", 24, 13, EC64Color::grey, EC64Color::red);
		}
		else
		{
			pGame->WriteText("      ", 24, 13, EC64Color::grey, EC64Color::red);
		}

		
		pGame->WriteText("sTART - space", 22, 20, EC64Color::grey, EC64Color::white);


		pGame->m_pC64->Redraw();
	}
	else if (m_nSelected == 1) //ball shop
	{
		EMaterial eMat;
		switch (m_nSelectedBall)
		{
		case 0:	eMat = EM_WOOD; break;
		case 1: eMat = EM_STONE;  break;
		case 2: eMat = EM_STEEL;  break;
		case 3: eMat = EM_FIRE;  break;
		case 4: eMat = EM_GOLD;  break;
		case 5: eMat = EM_EMERALD; break;
		}

		auto Pair = GetMaterialColor(eMat);

		std::string sName = GetMaterialName(eMat);
		sName += " ball";

		SC64Char Char;
		Char.cBG = Pair.first;
		Char.cFG = Pair.second;
		for (int x = 22; x < 34; ++x)
		{
			pGame->m_pC64->SetCharacter(x, 2, Char);
		}

		pGame->WriteText(sName, 22, 2, Pair.first, Pair.second);

		pGame->WriteText("<", 20, 2, cBG, cFG);
		pGame->WriteText(">", 35, 2, cBG, cFG);

		SBall Ball;
		Ball.SetMaterial(eMat);
		m_nPrice = Ball.m_nPrice;

		SGameSave& Save = pGame->GetSave();

		std::string sYouHave = "yOU HAVE: ";
		sYouHave += std::to_string(Save.m_aBalls[m_nSelectedBall]);
		pGame->WriteText(sYouHave, 22, 4, EC64Color::grey, EC64Color::white);

		pGame->WriteText("sTATS", 22, 6, EC64Color::grey, EC64Color::white);
		pGame->WriteText("pRICE", 22, 14, EC64Color::grey, EC64Color::white);

		Char.cBG = EC64Color::darkgrey;
		Char.cFG = EC64Color::white;
		for (int x = 22; x < 34; ++x)
		{
			for (int y = 7; y < 12; ++y)
			{
				pGame->m_pC64->SetCharacter(x, y, Char);
			}
		}

		std::string sStat = "hp:  " + std::to_string( Ball.m_nMapHp );
		pGame->WriteText(sStat, 23, 8, EC64Color::darkgrey, EC64Color::white);
		sStat = "dmg: " + std::to_string(Ball.m_nDamage);
		pGame->WriteText(sStat, 23, 9, EC64Color::darkgrey, EC64Color::white);
		sStat = "spd: " + std::to_string(Ball.m_nVelocity) + "-" + std::to_string(Ball.m_nMaxSpd);
		pGame->WriteText(sStat, 23, 10, EC64Color::darkgrey, EC64Color::white);


		for (int x = 22; x < 34; ++x)
		{
			for (int y = 15; y < 18; ++y)
			{
				pGame->m_pC64->SetCharacter(x, y, Char);
			}
		}

		std::string sPrice = std::to_string(Ball.m_nPrice) + " " + GetMaterialName(eMat);
		pGame->WriteText(sPrice, 23, 16, EC64Color::darkgrey, Pair.second);

		pGame->WriteDoubleNumber(35, 16, Save.m_aMaterials[m_nSelectedBall], Pair.first, Pair.second);

		pGame->WriteText("bUY - space", 22, 20, EC64Color::grey, EC64Color::white);

	}
	else if (m_nSelected == 2)
	{
		EMaterial eMat;
		switch (m_nSelectedBat)
		{
		case 0:	eMat = EM_WOOD; break;
		case 1: eMat = EM_STONE;  break;
		case 2: eMat = EM_STEEL;  break;
		case 3: m_nSelectedBat++; //ez nem lehet
		case 4: eMat = EM_GOLD;  break;
		case 5: eMat = EM_EMERALD; break;
		}

		auto Pair = GetMaterialColor(eMat);

		std::string sName = GetMaterialName(eMat);
		sName += " bat";

		SC64Char Char;
		Char.cBG = Pair.first;
		Char.cFG = Pair.second;
		for (int x = 22; x < 34; ++x)
		{
			pGame->m_pC64->SetCharacter(x, 2, Char);
		}

		pGame->WriteText(sName, 22, 2, Pair.first, Pair.second);

		SBat Bat;
		Bat.SetMaterial(eMat);
		m_nPrice = Bat.m_nPrice;

		SGameSave& Save = pGame->GetSave();

		std::string sYouHave = "yOU HAVE: ";
		sYouHave += std::to_string(Save.m_aBats[m_nSelectedBat]);
		pGame->WriteText(sYouHave, 22, 4, EC64Color::grey, EC64Color::white);

		pGame->WriteText("sTATS", 22, 6, EC64Color::grey, EC64Color::white);
		pGame->WriteText("pRICE", 22, 14, EC64Color::grey, EC64Color::white);

		Char.cBG = EC64Color::darkgrey;
		Char.cFG = EC64Color::white;
		for (int x = 22; x < 34; ++x)
		{
			for (int y = 7; y < 10; ++y)
			{
				pGame->m_pC64->SetCharacter(x, y, Char);
			}
		}

		std::string sStat = "hp:  " + std::to_string(Bat.m_nMapHp);
		pGame->WriteText(sStat, 23, 8, EC64Color::darkgrey, EC64Color::white);


		for (int x = 22; x < 34; ++x)
		{
			for (int y = 15; y < 18; ++y)
			{
				pGame->m_pC64->SetCharacter(x, y, Char);
			}
		}

		std::string sPrice = std::to_string(Bat.m_nPrice) + " " + GetMaterialName(eMat);
		pGame->WriteText(sPrice, 23, 16, EC64Color::darkgrey, Pair.second);

		pGame->WriteDoubleNumber(35, 16, Save.m_aMaterials[m_nSelectedBat], Pair.first, Pair.second);

		pGame->WriteText("bUY - space", 22, 20, EC64Color::grey, EC64Color::white);
	}
	else if (m_nSelected == 3)
	{
		pGame->WriteText("cREATED BY", 22, 7, EC64Color::grey, EC64Color::white);
		pGame->WriteText("hECTOR", 22, 8, EC64Color::grey, EC64Color::white);
		pGame->WriteText("2019", 22, 9, EC64Color::grey, EC64Color::white);
		pGame->WriteText("FOR", 22, 19, EC64Color::grey, EC64Color::white);
		pGame->WriteText("ready.", 22, 20, EC64Color::grey, EC64Color::white);
		pGame->WriteText("game.", 22, 21, EC64Color::grey, EC64Color::white);
		pGame->WriteText("jam.", 22, 22, EC64Color::grey, EC64Color::white);
	}
}