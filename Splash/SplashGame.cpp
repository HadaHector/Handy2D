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
#include "Assets.h"

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

	m_pAssetManager = new CAssetManager();
	m_pAssetManager->LoadAssetFile("assets/tree.json");

	SDLManager::SetClearColor(0, 0, 0, 1);
	
	CTexture::LoadTexture("resources/wall_left.png", "wall_left", true);
	CTexture::LoadTexture("resources/wall_right.png","wall_right", true);
	CTexture::LoadTexture("resources/box0.png", "box0");
	CTexture::LoadTexture("resources/box1.png", "box1");
	CTexture::LoadTexture("resources/box2.png", "box2");
	CTexture::LoadTexture("resources/box3.png", "box3");
	CTexture::LoadTexture("resources/fence0.png", "fence0");
	CTexture::LoadTexture("resources/fence1.png", "fence1");
	CTexture::LoadTexture("resources/fence2.png", "fence2");
	CTexture::LoadTexture("resources/fence3.png", "fence3");

	CTexture::LoadTexture("resources/selector.png", "selector", true);

	CTexture::LoadTexture("resources/tiles/0001.png", "tiles/0001", true);
	CTexture::LoadTexture("resources/tiles/0002.png", "tiles/0002", true);
	CTexture::LoadTexture("resources/tiles/0003.png", "tiles/0003", true);
	CTexture::LoadTexture("resources/tiles/0004.png", "tiles/0004", true);
	CTexture::LoadTexture("resources/tiles/0005.png", "tiles/0005", true);
	CTexture::LoadTexture("resources/tiles/0006.png", "tiles/0006", true);
	CTexture::LoadTexture("resources/tiles/0007.png", "tiles/0007", true);
	CTexture::LoadTexture("resources/tiles/0008.png", "tiles/0008", true);
	CTexture::LoadTexture("resources/tiles/0009.png", "tiles/0009", true);
	CTexture::LoadTexture("resources/tiles/0010.png", "tiles/0010", true);
	CTexture::LoadTexture("resources/tiles/0011.png", "tiles/0011", true);
	CTexture::LoadTexture("resources/tiles/0012.png", "tiles/0012", true);
	CTexture::LoadTexture("resources/tiles/0013.png", "tiles/0013", true);
	CTexture::LoadTexture("resources/tiles/0014.png", "tiles/0014", true);
	CTexture::LoadTexture("resources/tiles/0015.png", "tiles/0015", true);
	CTexture::LoadTexture("resources/tiles/0016.png", "tiles/0016", true);
	CTexture::LoadTexture("resources/tiles/0017.png", "tiles/0017", true);
	CTexture::LoadTexture("resources/tiles/0018.png", "tiles/0018", true);
	CTexture::LoadTexture("resources/tiles/0019.png", "tiles/0019", true);
	/*
	for (int i = 1; i <= 19; ++i)
	{
		std::ostringstream stringStream;
		stringStream << std::setfill('0') << std::setw(4) << i;
		std::string copyOfStr = stringStream.str();
		CTexture::LoadTexture("resources/tiles/" + copyOfStr + ".png", copyOfStr);
	}
	*/
	root = std::make_shared<CGameObject>();

	m_pTiledMap = new CTiledMap();
	SDLManager::Instance.AddLayer(&m_pTiledMap->m_Renderer);
	m_pTiledMap->m_Renderer.SetCamera({ 0,0 }, 0);

	m_pTiledMap->InitTestMap();
	m_pTiledMap->SetObjectToBuild(m_pAssetManager->GetAsset("tree"));

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

	m_pTiledMap->Update();

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