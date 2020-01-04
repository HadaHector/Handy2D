#include "Title.h"
#include "Breakout.h"
#include "Input.h"


CTitle::CTitle()
{
}


CTitle::~CTitle()
{
}

void CTitle::Activate()
{
	CBreakoutGame* pGame = CBreakoutGame::GetInstance();

	pGame->ClearScreen(EC64Color::darkgrey, EC64Color::white);

	SC64Char Char;
	Char.cBG = EC64Color::brown;
	Char.cFG = EC64Color::white;
	for (int x = 14; x < 24; ++x)
	{
		for (int y = 7; y < 12; ++y)
		{
			pGame->m_pC64->SetCharacter(x, y, Char);
		}
	}

	pGame->WriteText("strategy", 15, 8, EC64Color::brown, EC64Color::yellow);
	pGame->WriteText("breakout", 15, 10, EC64Color::brown, EC64Color::yellow);
	pGame->WriteText("pRESS space TO CONTINUE", 8, 24, EC64Color::darkgrey, EC64Color::white);



}

void CTitle::Update()
{
	if (Input::GetKey(KEY_SPACE).pressed)
	{
		CBreakoutGame* pGame = CBreakoutGame::GetInstance();
		pGame->SwitchState(Saves);
	}
}