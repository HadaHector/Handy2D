#include "SaveScreen.h"
#include "Breakout.h"
#include "Input.h"

void CSaveScreen::Activate()
{
	CBreakoutGame* pGame = CBreakoutGame::GetInstance();
	Refresh();
}

void CSaveScreen::Update()
{
	if (Input::GetKey(KEY_SPACE).pressed)
	{
		CBreakoutGame* pGame = CBreakoutGame::GetInstance();
		pGame->m_nActiveSaveSlot = m_nSelected;
		pGame->SwitchState(Shop);
	}
	if (Input::GetKey(KEY_RIGHT).pressed)
	{
		m_nSelected = (m_nSelected + 1) % 3;
		Refresh();
	}
	if (Input::GetKey(KEY_LEFT).pressed)
	{
		m_nSelected = (m_nSelected + 1) % 3;
		Refresh();
	}
}

void CSaveScreen::Refresh()
{
	CBreakoutGame* pGame = CBreakoutGame::GetInstance();

	pGame->ClearScreen(EC64Color::grey, EC64Color::orange);
	pGame->WriteText("sELECT SAVE SLOT", 12, 8, EC64Color::grey, EC64Color::white);

	for (int i = 0; i < 3; ++i)
	{
		bool bSel = m_nSelected == i;
		std::string sText = "slot";
		sText += std::to_string(i + 1);
		if (bSel) sText = "[" + sText + "]";
		pGame->WriteText(sText, 6+i*10 + (bSel ? 0 : 1), 13, EC64Color::grey, EC64Color::white);
	}
}