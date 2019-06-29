
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



struct SBall
{
	EMaterial m_eMaterial;
	int m_nHp, m_nMapHp;

	IntVec m_vPos, m_vSpeed;
	int m_nVelocity;
	int m_nDamage;
	int m_nMaxSpd;
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

	void SetMaterial(EMaterial eMat);

	bool m_bVisible = true;
};

class CBreakoutGame : public CGameLogic
{
public:
	// Inherited via CGameLogic
	virtual bool Load() override;

	void WriteText(const std::string& str, int x, int y);
	void WriteTopText(int x, int val, EC64Color cBG, EC64Color cFG);
	void WriteDoubleTopText(int x, int val, EC64Color cBG, EC64Color cFG );
	void DrawHpBar(int nPos, float fValue, EC64Color cBG, EC64Color cFG);

	SBrick* GetBrickAtPixel(IntVec pos);
	SC64Char LoadCharFromFile(const std::string& path);
	void LoadChars();
	void InitLevel();
	void SetCharacter(int x, int y, int nChar, EC64Color cBG, EC64Color cFG);
	void BounceOnBat();
	void HitBrick(SBrick& Brick);
	void NextBallMaterial();
	void NextBatMaterial();

	virtual void Update() override;

	int iFrameCounter = 0;
	double m_dFrametimer;

	CC64RenderLayer* m_pC64;

	std::vector<SBrick> m_aBricks;
	SBall m_Ball;
	SBat m_Bat;

	bool m_bBallOnBat;
	bool m_bTempBall;
	bool m_bTempBat;

	double m_dRespawnTime;

	void UpdateBall();
	void UpdateBat();
	
	int m_aCollectedMaterials[EM_LAST];
	int m_aBats[EM_LAST];
	int m_aBalls[EM_LAST];
	
};