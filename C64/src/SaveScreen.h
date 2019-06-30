#pragma once
#include "Base.h"


class CSaveScreen : public CBase
{
public:
	void Activate() override;
	void Update() override;
	void Refresh();

	int m_nSelected = 0;
};

