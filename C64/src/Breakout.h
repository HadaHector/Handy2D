#pragma once
#include "gamelogic.h"
#include "C64Renderer.h"
#include "Shop.h"
#include "Title.h"
#include "Game.h"


#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 16

//#define NO_FAIL

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

enum EGameState
{
	Title,
	Shop,
	Game
};


class CBreakoutGame : public CGameLogic
{
public:
	// Inherited via CGameLogic
	virtual bool Load() override;
	virtual void Update() override;

	static CBreakoutGame* GetInstance();

	void LoadChars();
	
	void LoadSave();
	void SetCharacter(int x, int y, int nChar, EC64Color cBG, EC64Color cFG);
	void WriteText(const std::string& str, int x, int y, EC64Color cBG, EC64Color cFG);
	void WriteTopText(int x, int val, EC64Color cBG, EC64Color cFG);
	void WriteDoubleTopText(int x, int val, EC64Color cBG, EC64Color cFG );
	void DrawHpBar(int nPos, float fValue, EC64Color cBG, EC64Color cFG);

	void ClearScreen(EC64Color cBG, EC64Color cFG);

	
	SC64Char LoadCharFromFile(const std::string& path);

	EGameState m_eState = Title;
	void SwitchState(EGameState eState);


	CC64RenderLayer* m_pC64;
	

	CTitle m_Title;
	CShop m_Shop;
	CGame m_Game;
};