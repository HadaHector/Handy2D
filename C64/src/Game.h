#pragma once
#include "Base.h"
#include "sdlmanager.h"
#include "C64Renderer.h"

enum EMaterial
{
	EM_NONE,
	EM_WOOD,
	EM_STONE,
	EM_STEEL,
	EM_FIRE,
	EM_GOLD,
	EM_EMERALD,
	EM_LAST
};


std::pair<EC64Color, EC64Color> GetMaterialColor(EMaterial mat);
std::string GetMaterialName(EMaterial mat);

struct SBall
{
	EMaterial m_eMaterial;
	int m_nHp, m_nMapHp;

	IntVec m_vPos, m_vSpeed;
	int m_nVelocity;
	int m_nDamage;
	int m_nMaxSpd;
	int m_nPrice;
	bool m_bVisible = true;

	int m_nBounceCount = 0;

	void SetMaterial(EMaterial eMat);
	void Bounce();
};


struct SBrick
{
	EMaterial m_eMaterial;
	int m_nHp;
	bool m_bDirty = false;

	SC64Char GetChar(bool bRight);
	void SetMaterial(EMaterial mat);
};


struct SBat
{
	EMaterial m_eMaterial;
	int m_nHp, m_nMapHp;

	int m_nPos;
	int m_nWidth;
	int m_nPrice;

	void SetMaterial(EMaterial eMat);

	bool m_bVisible = true;
};

class CC64RenderLayer;

class CGame : public CBase
{
public:
	CGame();
	~CGame();

	void Activate() override;
	void Update() override;

	CC64RenderLayer* GetRenderer();

	void StartLevel();
	void InitLevel();

	void BounceOnBat();
	void HitBrick(SBrick& Brick);
	void NextBallMaterial();
	void NextBatMaterial();

	SBrick* GetBrickAtPixel(IntVec pos);

	int iFrameCounter = 0;
	double m_dFrametimer;

	std::vector<SBrick> m_aBricks;
	SBall m_Ball;
	SBat m_Bat;

	bool m_bBallOnBat;
	bool m_bTempBall;
	bool m_bTempBat;

	double m_dRespawnTime;
	bool m_bGameOver = false;

	void UpdateBall();
	void UpdateBat();

	int m_aCollectedMaterials[EM_LAST];
	int m_aBats[EM_LAST];
	int m_aBalls[EM_LAST];
};

