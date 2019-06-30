#include "SDLManager.h"
#include "Breakout.h"
#include "Input.h"
#include "Audio.h"

#include <fstream>
#include <sstream>
#include <algorithm>


CBreakoutGame* CBreakoutGame::GetInstance()
{
	return (CBreakoutGame*)SDLManager::GetGameLogic();
}

bool CBreakoutGame::Load()
{
	Time::target_fps = 60;

	LoadChars();
	for (int i = 1; i <= 10; ++i)
	{
		CLevel Level;
		std::string sFile = "resources/level";
		sFile += std::to_string(i);
		sFile += ".txt";
		Level.Load(sFile);
		m_aLevels.push_back(Level);
	}

	CAudio::AllocateChannels(64);
	CSound::LoadSound("resources/bounce.wav");
	CSound::LoadSound("resources/win.wav");
	CSound::LoadSound("resources/loose.wav");
	CSound::LoadSound("resources/buy.wav");
	CSound::LoadSound("resources/nobuy.wav");
	CSound::LoadSound("resources/destroy.wav");

	m_pC64 = new CC64RenderLayer(IntRect(0, 0, 960, 600));
	SDLManager::Instance.AddLayer(m_pC64);

	LoadSaves();

	SwitchState(Title);
	return true;
}

void CBreakoutGame::Update()
{
	switch (m_eState)
	{
	case Title: m_Title.Update(); break;
	case Saves: m_SaveScreen.Update(); break;
	case Shop: m_Shop.Update(); break;
	case Game: m_Game.Update(); break;
	}
}

void CBreakoutGame::SwitchState(EGameState eState)
{
	m_eState = eState;
	switch (m_eState)
	{
	case Title: m_Title.Activate(); break;
	case Saves: m_SaveScreen.Activate(); break;
	case Shop: m_Shop.Activate(); break;
	case Game: m_Game.Activate(); break;
	}
}

void CBreakoutGame::LoadSaves()
{
	for (int i = 0; i < 3; ++i)
	{
		std::string sPath = "saves/save";
		sPath += std::to_string(i);
		sPath += ".sav";

		SGameSave& Save = m_aSaves[i];

		std::ifstream t(sPath);
		if (t.is_open())
		{
			std::stringstream buffer;
			buffer << t.rdbuf();
			buffer.read((char*)&Save.m_nHighestLevel,1);

			for (int x = 0; x < 6; ++x)
			{
				buffer.read((char*)&Save.m_aMaterials[x], 4);
				buffer.read((char*)&Save.m_aBalls[x], 4);
				buffer.read((char*)&Save.m_aBats[x], 4);
			}
		}
		else
		{
			Save.m_nHighestLevel = 1;
			for (int x = 0; x < 6; ++x)
			{
				Save.m_aMaterials[x] = 0;
				Save.m_aBalls[x] = 0;
				Save.m_aBats[x] = 0;
			}
			Save.m_aBalls[0] = 3;
			Save.m_aBats[0] = 3;
		}
	}
}

void CBreakoutGame::SaveSlot(int nSlot)
{
	std::string sPath = "saves/save";
	sPath += std::to_string(nSlot);
	sPath += ".sav";

	SGameSave& Save = m_aSaves[nSlot];


	std::ofstream t(sPath);
	if (t.is_open())
	{
		t.write((char*) &Save.m_nHighestLevel, 1);
		for (int x = 0; x < 6; ++x)
		{
			t.write((char*) &Save.m_aMaterials[x], 4);
			t.write((char*) &Save.m_aBalls[x], 4);
			t.write((char*) &	Save.m_aBats[x], 4);
		}
	}
}


std::vector<SC64Char> g_aLoadedChars;

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


void CBreakoutGame::SetCharacter(int x, int y, int nChar, EC64Color cBG, EC64Color cFG)
{
	SC64Char Char = g_aLoadedChars[nChar];
	Char.cBG = cBG;
	Char.cFG = cFG;
	m_pC64->SetCharacter(x, y, Char);
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





void CBreakoutGame::WriteText(const std::string & str, int x, int y, EC64Color cBG, EC64Color cFG)
{
	for (unsigned int i = 0; i < str.size(); ++i)
	{
		if (x + i > 39) return;
		SC64Char Char = m_pC64->GetCharacter(x + i, y);
		Char.SetCharacter(str[i], false);
		Char.cBG = cBG;
		Char.cFG = cFG;
		m_pC64->SetCharacter(x + i, y, Char);
	}
}




void CBreakoutGame::WriteNumber(int x, int y, int val, EC64Color cBG, EC64Color cFG)
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

	m_pC64->SetCharacter(x, y, Char);
}

void CBreakoutGame::WriteDoubleNumber(int x, int y, int val, EC64Color cBG, EC64Color cFG)
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

	m_pC64->SetCharacter(x+1, y, Char1);

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

	m_pC64->SetCharacter(x, y, Char2);
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

void CBreakoutGame::ClearScreen(EC64Color cBG, EC64Color cFG)
{
	for (int x = 0; x < 40; ++x)
	{
		for (int y = 0; y < 25; ++y)
		{
			SC64Char Char;
			Char.cBG = cBG;
			Char.cFG = cFG;
			m_pC64->SetCharacter(x,y, Char);
		}
	}
}
