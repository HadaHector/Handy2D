#pragma once
#include "SDLManager.h"
#include "RenderLayer.h"
#include <array>
#include "Input.h"
#include <algorithm>
#include "GameObject.h"
#include "GuiLayer.h"
#include <stdlib.h>


class CSplashGame;


class CSplashGame : public CGameLogic
{
	std::vector<PGuiImage> aImages;
	std::shared_ptr<CGameObject> root;
	CGuiLayer *pGui;
	class CTiledMap *m_pTiledMap;

	bool m_bResized = false;
	static CSplashGame* m_pInstance;

	PGuiElement pMenu;

	class CAssetManager* m_pAssetManager;

public:
	CSplashGame() { m_pInstance = this; }
	virtual bool Load() override;
	void Resize();
	void UpdateGui();
	virtual void Update() override;
	virtual void OnResize() override;
	static CSplashGame* GetInstance() { return m_pInstance; }
};

