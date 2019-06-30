#pragma once
#include "Base.h"


class CShop : public CBase
{
public:
	CShop();
	~CShop();

	void Activate() override;
	void Update() override;
};

