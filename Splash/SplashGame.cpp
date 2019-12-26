#pragma once
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
	std::uniform_int_distribution<int> random_4(0, 3);
}








CSplashGame* CSplashGame::m_pInstance;

bool CSplashGame::Load()
{
	Time::target_fps = 120;

	SDLManager::SetClearColor(0, 0, 0, 1);
	
	CTexture::LoadTexture("resources/test_tile.png", "tile");
	CTexture::LoadTexture("resources/test_tile2.png", "tile2");
	CTexture::LoadTexture("resources/tree.png", "tree");
	CTexture::LoadTexture("resources/wall_left.png", "wall_left");
	CTexture::LoadTexture("resources/wall_right.png","wall_right");
	CTexture::LoadTexture("resources/box0.png", "box0");
	CTexture::LoadTexture("resources/box1.png", "box1");
	CTexture::LoadTexture("resources/box2.png", "box2");
	CTexture::LoadTexture("resources/box3.png", "box3");
	CTexture::LoadTexture("resources/fence0.png", "fence0");
	CTexture::LoadTexture("resources/fence1.png", "fence1");
	CTexture::LoadTexture("resources/fence2.png", "fence2");
	CTexture::LoadTexture("resources/fence3.png", "fence3");
	root = std::make_shared<CGameObject>();

	m_pTiledMap = new CTiledMap();
	SDLManager::Instance.AddLayer(&m_pTiledMap->m_Renderer);
	m_pTiledMap->m_Renderer.SetCamera({ 0,0 }, 0);

	m_pTiledMap->InitTestMap();

	pGui = new CGuiLayer(IntRect(0, 0, 1024, 512));
	SDLManager::Instance.AddLayer(pGui);

	return true;
}


void CSplashGame::Resize()
{
	IntVec vWinSize = SDLManager::GetSize();
	pGui->SetRect(IntRect(0, 0, vWinSize.x, vWinSize.y));
	m_pTiledMap->m_Renderer.SetRect(IntRect(0, 0, vWinSize.x, vWinSize.y));
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

	m_pTiledMap->m_Renderer.UpdateMovement();

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