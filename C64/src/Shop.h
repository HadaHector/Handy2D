#pragma once
#include "Base.h"


class CShop : public CBase
{
public:
	CShop();
	~CShop();

	void Activate() override;
	void Update() override;
	void Refresh();

	int m_nSelected = 0;
	int m_nSelectedLevel = 0;
	int m_nSelectedBall = 0;
	int m_nSelectedBat = 0;

	bool m_bLevelLocked = false;

	int m_nPrice = 0;
};

