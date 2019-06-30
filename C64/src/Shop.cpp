#include "Shop.h"
#include "Breakout.h"
#include "Input.h"


CShop::CShop()
{
}


CShop::~CShop()
{
}

void CShop::Activate()
{
	CBreakoutGame* pGame = CBreakoutGame::GetInstance();

	EC64Color cBG = EC64Color::grey;
	EC64Color cFG = EC64Color::orange;
	pGame->ClearScreen(cBG, cFG);
	pGame->WriteText("shop", 18, 1, cBG, cFG);
}

void CShop::Update()
{
	if (Input::GetKey(KEY_SPACE).pressed)
	{
		CBreakoutGame* pGame = CBreakoutGame::GetInstance();
		pGame->SwitchState(Game);
	}
}