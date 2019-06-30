#pragma once
#include "Base.h"


class CTitle : public CBase
{
public:
	CTitle();
	~CTitle();

	void Activate() override;
	void Update() override;
};

