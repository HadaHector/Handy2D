
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


struct SBrick
{
	EMaterial m_eMaterial;
	int m_nHp;
	bool m_bDirty = false;

	SC64Char GetChar(bool bRight);
	void SetMaterial(EMaterial mat);
	void Hit();
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

	virtual void Update() override;

	int iTest = 0;
	double m_dFrametimer;

	CC64RenderLayer* m_pC64;


	IntVec m_vBallPos, m_vBallSpeed;
	int m_nBallVelocity;

	std::vector<SBrick> m_aBricks;

	void UpdateBall();
};