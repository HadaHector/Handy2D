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
	CTexture::LoadTexture("resources/tree.png","tree");
	CTexture::LoadTexture("resources/box0.png", "box0");
	CTexture::LoadTexture("resources/box1.png", "box1");
	CTexture::LoadTexture("resources/box2.png", "box2");
	CTexture::LoadTexture("resources/box3.png", "box3");
	CTexture::LoadTexture("resources/fence0.png", "fence0");
	CTexture::LoadTexture("resources/fence1.png", "fence1");
	CTexture::LoadTexture("resources/fence2.png", "fence2");
	CTexture::LoadTexture("resources/fence3.png", "fence3");
	root = std::make_shared<CGameObject>();

	m_pTiledMap = new CTiledMap(IntRect(0, 0, 1024, 512));
	SDLManager::Instance.AddLayer(m_pTiledMap);
	m_pTiledMap->SetCamera({ 0,0 }, 0);

	IntVec vMapSize = { 40,40 };

	m_pTiledMap->SetSize(vMapSize);
	for (int y = 0; y < vMapSize.y; ++y)
	{
		for (int x = 0; x < vMapSize.x; ++x)
		{
			int nHeight = 0; // random_4(e1) == 0 ? 1 : 0;

			std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
			pSprite->SetTexture(CTexture::GetTexture( x%4==0 ? "tile2" : "tile"));
			pSprite->SetSize({ 64,32 });
			m_pTiledMap->AddSprite({ x,y }, STileData::Ground, { pSprite, nHeight * 16 });
			if (random_4(e1) == 0)
			{
				std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
				pSprite->SetTexture(CTexture::GetTexture("tree"));
				pSprite->SetSize({ 64,64 });
				m_pTiledMap->AddSprite({ x,y }, STileData::Object, { pSprite, 32 + nHeight * 16, random_4(e1) });
			}
			else if (random_4(e1) == 0)
			{
				std::vector<std::shared_ptr<CSprite>> aSprites;
				for (int i = 0; i < 4; ++i)
				{
					std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
					pSprite->SetTexture(CTexture::GetTexture(std::string("box") + std::to_string(i)));
					pSprite->SetSize({ 64,64 });
					aSprites.push_back(pSprite);
				}
				m_pTiledMap->AddSprite({ x,y }, STileData::Object, { aSprites, 32 + nHeight * 16, random_4(e1) });
			}

			if (random_4(e1) == 0)
			{
				std::vector<std::shared_ptr<CSprite>> aSprites;
				for (int i = 0; i < 4; ++i)
				{
					std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
					pSprite->SetTexture(CTexture::GetTexture(std::string("fence") + std::to_string(i)));
					pSprite->SetSize({ 64,48 });
					aSprites.push_back(pSprite);
				}
				int nSide = random_4(e1);
				m_pTiledMap->AddSprite({ x,y }, (STileData::ETileSpriteSlot) ((int)STileData::Side0 + nSide), { aSprites, 16, nSide });
			}
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