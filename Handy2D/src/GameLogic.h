#pragma once

class CGameLogic
{
public:
	virtual bool Load() = 0;
	virtual void Update() = 0;
	virtual void OnResize() {};
	bool m_bRunning = true;
};

