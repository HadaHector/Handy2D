
#include "C64Renderer.h"


enum EMaterial
{
	EM_NONE,
	EM_WOOD,
	EM_STONE,
	EM_STEEL,
	EM_FIRE,
	EM_GOLD,
	EM_EMERALD
};



struct SBall
{
	EMaterial m_eMaterial;
	int m_nHp, m_nMapHp;

	IntVec m_vPos, m_vSpeed;
	int m_nVelocity;
	int m_nDamage;

	void Bounce();
};


struct SBrick
{
	EMaterial m_eMaterial;
	int m_nHp;
	bool m_bDirty = false;

	SC64Char GetChar(bool bRight);
	void SetMaterial(EMaterial mat);
	void Hit(SBall& Ball);
};


struct SBat
{
	EMaterial m_eMaterial;
	int m_nHp, m_nMapHp;

	int m_nPos;
	int m_nWidth;

	void Bounce(SBall& Ball);
};

class CBreakoutGame : public CGameLogic
{
public:
	// Inherited via CGameLogic
	virtual bool Load() override;

	void WriteText(const std::string& str, int x, int y);

	SBrick* GetBrickAtPixel(IntVec pos);
	SC64Char LoadCharFromFile(const std::string& path);
	void LoadChars();
	void InitLevel();
	void SetCharacter(int x, int y, int nChar, EC64Color cBG, EC64Color cFG);

	virtual void Update() override;

	int iTest = 0;
	double m_dFrametimer;

	CC64RenderLayer* m_pC64;

	std::vector<SBrick> m_aBricks;
	SBall m_Ball;
	SBat m_Bat;

	void UpdateBall();
	void UpdateBat();
};