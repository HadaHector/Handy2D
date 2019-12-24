﻿#pragma once
#include "SplashGame.h"
#include "SDLManager.h"
#include "RenderLayer.h"
#include <array>
#include "Input.h"
#include <algorithm>
#include "GameObject.h"
#include "GuiLayer.h"
#include <stdlib.h>
#include "CollisionManager.h"
#include <random>
#include "PaintedImage.h"
#include "Audio.h"
#include "TiledMapRenderer.h"

const double PI = 3.141592653589793238463;

namespace {
	std::random_device r;
	std::default_random_engine e1(r());
	//std::uniform_int_distribution<int> random_4(0, 3);
}








CSplashGame* CSplashGame::m_pInstance;

bool CSplashGame::Load()
{
	Time::target_fps = 120;

	SDLManager::SetClearColor(0, 0, 0, 1);
	
	CTexture::LoadTexture("resources/test_tile.png","tile");
	root = std::make_shared<CGameObject>();

	m_pTiledMap = new CTiledMap(IntRect(0, 0, 1024, 512));
	SDLManager::Instance.AddLayer(m_pTiledMap);
	m_pTiledMap->SetCamera({ -5,-2 }, 0);

	IntVec vMapSize = { 20,20 };

	m_pTiledMap->SetSize(vMapSize);
	for (int y = 0; y < vMapSize.y; ++y)
	{
		for (int x = 0; x < vMapSize.x; ++x)
		{
			std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
			pSprite->SetTexture(CTexture::GetTexture("tile"));
			pSprite->SetSize({ 64,32 });
			m_pTiledMap->AddSprite({ x,y }, { pSprite });
		}
	}

	pGui = new CGuiLayer(IntRect(0, 0, 1024, 512));
	SDLManager::Instance.AddLayer(pGui);

	return true;
}


void CSplashGame::Resize()
{
	IntVec vWinSize = SDLManager::GetSize();
	pGui->SetRect(IntRect(0, 0, vWinSize.x, vWinSize.y));
	m_pTiledMap->SetRect(IntRect(0, 0, vWinSize.x, vWinSize.y));
	//pMenuBg->SetSize(vWinSize);
	m_bResized = false;
}

void CSplashGame::Update()
{
	root->UpdateInternal(Vec(0, 0));

	if (m_bResized)
	{
		Resize();
	}

	m_pTiledMap->UpdateMovement();

	UpdateGui();
	//pFPS->SetText("fps:" + std::to_string(Time::fps) + " sprites:" + std::to_string(SDLManager::drawnum));
}

void CSplashGame::UpdateGui()
{

}

void CSplashGame::OnResize()
{
	m_bResized = true;
}