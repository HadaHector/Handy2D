#pragma once
class CBase
{
public:

	virtual void Activate() {}
	virtual void Update() {}
	virtual bool Load() { return true; }
};

