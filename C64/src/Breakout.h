
#include "C64Renderer.h"



class CBreakoutGame : public CGameLogic
{
public:
	// Inherited via CGameLogic
	virtual bool Load() override;

	void WriteText(const std::string& str, int x, int y);

	virtual void Update() override;

	int iTest = 0;
	double m_dFrametimer;

	CC64RenderLayer* m_pC64;


	IntVec m_vBallPos, m_vBallSpeed;

	void UpdateBall();
};