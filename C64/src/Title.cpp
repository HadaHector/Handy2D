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

	pGame->ClearScreen(EC64Color::grey, EC64Color::orange);
	pGame->WriteText("strategy", 15, 8, EC64Color::grey, EC64Color::black);
	pGame->WriteText("breakout", 15, 10, EC64Color::grey, EC64Color::black);
	pGame->WriteText("pRESS space TO CONTINUE", 8, 24, EC64Color::grey, EC64Color::white);
}

void CTitle::Update()
{
	if (Input::GetKey(KEY_SPACE).pressed)
	{
		CBreakoutGame* pGame = CBreakoutGame::GetInstance();
		pGame->SwitchState(Saves);
	}
}