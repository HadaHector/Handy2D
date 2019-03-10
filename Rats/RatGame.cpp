#pragma once
#include "RatGame.h"
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

const double PI = 3.141592653589793238463;

namespace {
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> random_4(0, 3);
	std::uniform_real_distribution<float> random_float(0, 1);
	std::uniform_int_distribution<int> random_bool(0, 1);
}

CRatGame* CRatGame::m_pInstance;


void CImpact::Init()
{
	std::shared_ptr<CAnimSprite> pEff = std::make_shared<CAnimSprite>();
	pEff->SetTexture(CTexture::LoadTexture("resources/impact.png"));
	pEff->SetSize(IntVec(64, 64));
	pEff->SetRowsAndCols(2, 2);
	pEff->SetFrameTime(0.1);
	AttachSprite(pEff, Vec(-16, -16), { "camera" });
	CAudio::PlaySound(CSound::GetSound("resources/hit.wav"));
}

void CImpact::Update()
{
	m_fLifetime += (float) Time::frame;
	if (m_fLifetime > 0.2f)
	{
		DestroySelf();
	}
}

void CBase::SetPlayer(int nPlayer)
{
	m_nPlayer = nPlayer;
	if (m_pBase)
	{
		m_pBase->SetTexture(CTexture::GetTexture(m_nPlayer ? "resources/base_blue.png" : "resources/base_green.png"));
		m_pBase->SetSize(144 * 2, 176 * 2);
	}
}

void CBase::Init()
{
	CTexture::LoadTexture("resources/base_bg.png");
	CTexture::LoadTexture("resources/base_blue.png");
	CTexture::LoadTexture("resources/base_green.png");
	
	std::shared_ptr<CImageSprite> pBg = std::make_shared<CImageSprite>();
	pBg->SetTexture(CTexture::GetTexture("resources/base_bg.png"));
	pBg->SetSize(144*2,176*2);
	AttachSprite(pBg, Vec(0,0), { "bg" });
	

	m_pBase = std::make_shared<CImageSprite>();
	m_pBase->SetTexture(CTexture::GetTexture(m_nPlayer ? "resources/base_blue.png" : "resources/base_green.png"));
	m_pBase->SetSize(144 * 2, 176 * 2);
	AttachSprite(m_pBase, Vec(0,0), { "camera" });

	AddCollider(IntRect(0, 6, 51, 35) * 2, false, OVERLAP | COLLIDE);
	AddCollider(IntRect(93, 6, 51, 35) * 2, false, OVERLAP | COLLIDE);
	AddCollider(IntRect(0, 141, 51, 35) * 2, false, OVERLAP | COLLIDE);
	AddCollider(IntRect(93, 141, 51, 35) * 2, false, OVERLAP | COLLIDE);
	AddCollider(IntRect(0, 41, 9, 100) * 2, false, OVERLAP | COLLIDE);
	AddCollider(IntRect(135, 42, 9, 100) * 2, false, OVERLAP | COLLIDE);
}



void CRechargeArea::Init()
{
	AddCollider(IntRect(0, 32, 144, 144) * 2, false, OVERLAP );
}


void CProjectile::Init()
{
	CTexture::LoadTexture("resources/projectile.png");
	std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
	pSprite->SetTexture(CTexture::GetTexture("resources/projectile.png"));
	pSprite->SetSize(8, 8);
	AttachSprite(pSprite, Vec(-4, -4), { "camera" });
	AddCollider(IntRect(-4, -4, 8, 8), true, OVERLAP | OVERLAP_EVENTS );
}

void CProjectile::Update()
{
	AddMovement(m_vSpeed * Time::frame);
}

void CProjectile::SetSpeed(const IntVec& spd)
{
	m_vSpeed = spd;
}

void CProjectile::SetPlayer(int nPlayer)
{
	m_nPlayer = nPlayer;
}

void CProjectile::Explode(const Vec& pos)
{
	IntVec coord = pos / TILE_SIZE;
	CRatGame::GetInstance()->SetTile(coord, ET_Empty);

	static IntVec dirs[8] = { {1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1} };
	for (IntVec dir : dirs)
	{
		if (random_float(e1) > 0.25f)
		{
			CRatGame::GetInstance()->BreakTile(coord + dir);
			if (random_float(e1) < 0.33)
			{
				CRatGame::GetInstance()->BreakTile(coord + (dir * 2));
			}
		}
	}
}

bool CProjectile::CollideTerrain(const Vec& pos)
{
	ETile tile = CRatGame::GetInstance()->GetTileAtPos(pos);
	if (tile == ET_Rock)
	{
		auto pImpact = std::make_shared<CImpact>();
		GetParent()->AttachGameObject(pImpact, GetAbsolutePos());
		DestroySelf();
		return true;
	}
	if (tile >= ET_Dirt1 && tile <= ET_Dirt4)
	{
		auto pImpact = std::make_shared<CImpact>();
		GetParent()->AttachGameObject(pImpact, GetAbsolutePos());
		Explode(pos);
		DestroySelf();

		return true;
	}
	return false;
}

void CProjectile::OnMove(const Vec& from, const Vec& to)
{
	int count = (int) ((to - from).GetLength() / TILE_SIZE);
	Vec normalized = (from - to).GetNormalized();
	for (int i = 0; i < count; ++i)
	{
		Vec pos = from + (i * normalized * TILE_SIZE);
		if (CollideTerrain(pos))
		{
			return;
		}
	}
	if (CollideTerrain(to))
	{
		return;
	}
}

void CProjectile::OnOverlap(const SOverlapEvent &event)
{

	CRat* pRat = dynamic_cast<CRat*>(event.pOther.get());
	if (pRat && pRat->m_nPlayer != m_nPlayer)
	{
		pRat->Damage(10);
		DestroySelf();
		return;
	}

	CBase* pBase = dynamic_cast<CBase*>(event.pOther.get());
	if (pBase)
	{
		DestroySelf();
		return;
	}
}



void CRat::Init()
{
	CTexture::LoadTexture("resources/rat_left.png");
	CTexture::LoadTexture("resources/rat_right.png");
	CTexture::LoadTexture("resources/rat_up.png");
	CTexture::LoadTexture("resources/rat_down.png");
	CTexture::LoadTexture("resources/rat_upleft.png");
	CTexture::LoadTexture("resources/rat_upright.png");
	CTexture::LoadTexture("resources/rat_downleft.png");
	CTexture::LoadTexture("resources/rat_downright.png");

	CTexture::LoadTexture("resources/rat_left2.png");
	CTexture::LoadTexture("resources/rat_right2.png");
	CTexture::LoadTexture("resources/rat_up2.png");
	CTexture::LoadTexture("resources/rat_down2.png");
	CTexture::LoadTexture("resources/rat_upleft2.png");
	CTexture::LoadTexture("resources/rat_upright2.png");
	CTexture::LoadTexture("resources/rat_downleft2.png");
	CTexture::LoadTexture("resources/rat_downright2.png");

	m_pRat = std::make_shared<CAnimSprite>();
	if (m_nPlayer == 1)
	{
		m_pRat->SetTexture(CTexture::GetTexture("resources/rat_left.png"));
	}
	else
	{
		m_pRat->SetTexture(CTexture::GetTexture("resources/rat_left2.png"));
	}
	m_pRat->SetRowsAndCols(2, 1);
	m_pRat->SetSize(80, 80);
	AttachSprite(m_pRat, Vec(-40, -40), { "camera" });
	AddCollider(IntRect(-24, -24, 48, 48), true, OVERLAP | OVERLAP_EVENTS | COLLIDE | COLLIDE_EVENTS);
	AddSocket("shoot", Vec(0, 0));
	m_vDir = { -1,0 };
	m_bDead = false;
}

void CRat::Fire()
{
	auto bullet = std::make_shared<CProjectile>();
	bullet->SetSpeed(m_vDir * 500);
	bullet->SetPlayer(m_nPlayer);
	GetParent()->AttachGameObject(bullet, GetSocketAbsPos("shoot"));

	m_fEnergy -= 0.5f;
	if (m_fEnergy <= 0)
	{
		Die();
	}

	CAudio::PlaySound(CSound::GetSound("resources/laser.wav"));
}

void CRat::Update()
{
	bool bMoving = false;
	if (m_freezetimer < Time::frame)
	{
		double time = Time::frame - m_freezetimer;		
		bool bLeft = Input::GetKey(m_nPlayer ? KEY_LEFT : KEY_A).active; //KEY_KP_4
		bool bRight = Input::GetKey(m_nPlayer ? KEY_RIGHT : KEY_D).active; //KEY_KP_6
		bool bUp = Input::GetKey(m_nPlayer ? KEY_UP : KEY_W).active; //KEY_KP_8
		bool bDown = Input::GetKey(m_nPlayer ? KEY_DOWN : KEY_S).active; //KEY_KP_5

		std::string tex;
		if (bUp)
		{
			if (bLeft)
			{
				m_vDir = { -1,-1 };
				tex = "resources/rat_upleft";
			}
			else if (bRight)
			{
				m_vDir = { 1,-1 };
				tex = "resources/rat_upright";
			}
			else
			{
				m_vDir = { 0,-1 };
				tex = "resources/rat_up";
			}
		}
		else if (bDown)
		{
			if (bLeft)
			{
				m_vDir = { -1,1 };
				tex = "resources/rat_downleft";
			}
			else if (bRight)
			{
				m_vDir = { 1,1 };
				tex = "resources/rat_downright";
			}
			else
			{
				m_vDir = { 0,1 };
				tex = "resources/rat_down";
			}
		}
		else if (bLeft)
		{
			m_vDir = { -1,0 };
			tex = "resources/rat_left";
		}
		else if (bRight)
		{
			m_vDir = { 1,0 };
			tex = "resources/rat_right";
		}

		if (tex.size() > 0)
		{
			m_vDir.Normalize();
			AddMovement(m_vDir * Time::frame * 300.0f);
			if (m_nPlayer == 1)
			{
				tex += ".png";
			}
			else
			{
				tex += "2.png";
			}
			m_pRat->SetTexture(CTexture::GetTexture(tex));
			m_pRat->SetSize(80, 80);
			bMoving = true;
		}
	}

	m_freezetimer = std::max(0.0f, (float)( m_freezetimer - Time::frame));

	if ((m_nPlayer ? (Input::GetKey(KEY_KP_ENTER).active || Input::GetKey(KEY_RETURN).active) : Input::GetKey(KEY_SPACE).active) && m_cooldown == 0)
	{
		Fire();
		m_cooldown = 0.15;
	}

	if (bMoving)
	{
		m_timer += Time::frame * 200;
	}

	if (m_timer > 20)
	{
		m_nStep = (m_nStep + 1) % 2;
		m_timer = 0;
		m_pRat->SetFrame(m_nStep);
	}

	m_cooldown = std::max(double(0), m_cooldown - Time::frame);

	m_fEnergy -= (float)(Time::frame * 0.5);
	if (m_fEnergy <= 0)
	{
		Die();
	}
}

void CRat::Die()
{
	if (!m_bDead)
	{
		DestroySelf();
		CRatGame::GetInstance()->AddScore(m_nPlayer ? 0 : 1);
		CRatGame::GetInstance()->StartExplosion(GetPos());
		CAudio::PlaySound(CSound::GetSound("resources/explosion.wav"));
		m_bDead = true;
	}
	
}

void CRat::Damage(int nDmg)
{
	m_fHealth -= nDmg;
	if (m_fHealth <= 0) Die();
}

void CRat::OnMove(const Vec& from, const Vec& to)
{
	int nFreeze = 0;
	//gonosz modon feltetelezem, hogy lesz annyi fps, hogy ne menjen 64-nel tobbet egy frame alatt

	for (int y = -16; y <= 16; y += 16)
		for (int x = -16; x <= 16; x += 16)
		{
			Vec pos = to;
			pos.x += x;
			pos.y += y;
			ETile t = CRatGame::GetInstance()->GetTileAtPos(pos);
			if (t == ET_Rock)
			{
				SetPosition(from);
				return;
			}
			if (t >= ET_Dirt1 && t <= ET_Dirt4)
			{
				CRatGame::GetInstance()->SetTileAtPos(pos, ET_Empty);
				nFreeze++;
			}
		}
	
	if (nFreeze>0)
	{
		Freeze(std::max(nFreeze*0.05f,0.0f));
	}
	
}

void CRat::Freeze(float time)
{
	m_freezetimer += time;
}

void CRat::OnOverlap(const SOverlapEvent& Event)
{
	CRechargeArea* pArea = dynamic_cast<CRechargeArea*>(Event.pOther.get());
	if (pArea && pArea->m_nPlayer == m_nPlayer)
	{
		m_fEnergy = std::min(m_fMaxEnergy, m_fEnergy + (float)Time::frame * 5.0f);
		m_fHealth = std::min(m_fMaxHealth, m_fHealth + (float)Time::frame * 2.0f);
	}
}

ETile CRatGame::GetTile(IntVec pos)
{
	if (pos.x < 0 || pos.y < 0 || pos.x >= MAP_SIZE_X || pos.y >= MAP_SIZE_Y)
	{
		return ET_Rock;
	}
	return mapdata[pos.y][pos.x];
}

void CRatGame::BreakTile(IntVec pos)
{
	if (pos.x < 0 || pos.y < 0 || pos.x >= MAP_SIZE_X || pos.y >= MAP_SIZE_Y)
	{
		return;
	}
	ETile t = mapdata[pos.y][pos.x];
	if (t >= ET_Dirt1 && t <= ET_Dirt4)
	{
		mapdata[pos.y][pos.x] = ET_Empty;
	}
	
}

void CRatGame::BreakTileAtPos(Vec pos)
{
	IntVec coord = pos / TILE_SIZE;
	BreakTile(coord);
}

void CRatGame::SetTile(IntVec pos, ETile tile)
{
	if (pos.x < 0 || pos.y < 0 || pos.x >= MAP_SIZE_X || pos.y >= MAP_SIZE_Y)
	{
		return;
	}
	mapdata[pos.y][pos.x] = tile;
}

ETile CRatGame::GetTileAtPos(Vec pos)
{
	IntVec coord = pos / TILE_SIZE;
	return GetTile(coord);
}

void CRatGame::SetTileAtPos(Vec pos, ETile tile)
{
	IntVec coord = pos / TILE_SIZE;
	SetTile(coord, tile);
}


bool CRatGame::Load()
{
	Time::target_fps = 120;

	m_DirtLayer1.Load();
	m_DirtLayer2.Load();
	pCamera1 = new CCameraRenderLayer(IntRect(0, 0, 512, 512));
	pCamera2 = new CCameraRenderLayer(IntRect(512, 0, 512, 512));
	pBgCamera1 = new CCameraRenderLayer(IntRect(0, 0, 512, 512));
	pBgCamera2 = new CCameraRenderLayer(IntRect(512, 0, 512, 512));
	m_DirtLayer1.pDirtLayer = new CSpriteRenderLayer(IntRect(0, 0, 512, 512));
	m_DirtLayer2.pDirtLayer = new CSpriteRenderLayer(IntRect(512, 0, 512, 512));
	pCamera1->AddTag("camera");
	pCamera1->SetName("main1");
	pCamera2->AddTag("camera");
	pCamera2->SetName("main2");
	pBgCamera1->AddTag("bg");
	pBgCamera2->AddTag("bg");
	pBgCamera1->SetName("bg1");
	pBgCamera2->SetName("bg2");
	m_DirtLayer1.pDirtLayer->SetName("dirt1");
	m_DirtLayer2.pDirtLayer->SetName("dirt2");
	pCamera2->SetPosition(IntVec(0, 0));
	SDLManager::Instance.AddLayer(pBgCamera1);
	SDLManager::Instance.AddLayer(pBgCamera2);
	SDLManager::Instance.AddLayer(m_DirtLayer1.pDirtLayer);
	SDLManager::Instance.AddLayer(m_DirtLayer2.pDirtLayer);
	SDLManager::Instance.AddLayer(pCamera1);
	SDLManager::Instance.AddLayer(pCamera2);

	CTexture::LoadTexture("resources/bg.png");

	CAudio::AllocateChannels(64);
	CSound::LoadSound("resources/laser.wav");
	CSound::LoadSound("resources/explosion.wav");
	CSound::LoadSound("resources/hit.wav");

	for (int x = -1; x < 3; ++x)
	{
		for (int y = -1; y < 3; ++y)
		{
			PImageSprite pBg = PNEW(CImageSprite);
			pBg->SetTexture(CTexture::GetTexture("resources/bg.png"));
			pBg->SetSize(2048, 2048);
			pBg->SetPos(Vec(2048 * x, 2048 * y));
			pBgCamera1->AddSprite(pBg);
			pBgCamera2->AddSprite(pBg);
			m_aBgs.push_back(pBg);
		}
	}


	root = std::make_shared<CGameObject>();

	Vec Size = SDLManager::Instance.GetSize();



	pGui = new CGuiLayer(IntRect(0, 0, 1024, 512));
	SDLManager::Instance.AddLayer(pGui);


	pMapImage = new CPaintedImage(MAP_SIZE_X, MAP_SIZE_Y);

	CTexture::LoadTexture("resources/barbg.png");
	CTexture::LoadTexture("resources/hpbar.png");
	CTexture::LoadTexture("resources/energybar.png");
	CTexture::LoadTexture("resources/energy.png");
	CTexture::LoadTexture("resources/health.png");
	CTexture::LoadTexture("resources/black.png");

	for (int i = 0; i < 5; ++i)
	{

		pBg[i] = PNEW(CGuiImage);
		pBg[i]->SetImage("resources/black.png");
		pGui->GetRootElement()->AddChild(pBg[i]);
	}


	pLeftBar1Bg = PNEW(CGuiImage);
	pLeftBar1Bg->SetImage("resources/barbg.png");
	pLeftBar2Bg = PNEW(CGuiImage);
	pLeftBar2Bg->SetImage("resources/barbg.png");
	pRightBar1Bg = PNEW(CGuiImage);
	pRightBar1Bg->SetImage("resources/barbg.png");
	pRightBar2Bg = PNEW(CGuiImage);
	pRightBar2Bg->SetImage("resources/barbg.png");

	pLeftHpBar = PNEW(CGuiImage);
	pLeftHpBar->SetImage("resources/hpbar.png");
	pRightHpBar = PNEW(CGuiImage);
	pRightHpBar->SetImage("resources/hpbar.png");
	pLeftEnergyBar = PNEW(CGuiImage);
	pLeftEnergyBar->SetImage("resources/energybar.png");
	pRightEnergyBar = PNEW(CGuiImage);
	pRightEnergyBar->SetImage("resources/energybar.png");

	pLeftEnergyIcon = PNEW(CGuiImage);
	pLeftEnergyIcon->SetImage("resources/energy.png");
	pRightEnergyIcon = PNEW(CGuiImage);
	pRightEnergyIcon->SetImage("resources/energy.png");
	pLeftHealthIcon = PNEW(CGuiImage);
	pLeftHealthIcon->SetImage("resources/health.png");
	pRightHealthIcon = PNEW(CGuiImage);
	pRightHealthIcon->SetImage("resources/health.png");

	pGui->GetRootElement()->AddChild(pLeftBar1Bg);
	pGui->GetRootElement()->AddChild(pLeftBar2Bg);
	pGui->GetRootElement()->AddChild(pRightBar1Bg);
	pGui->GetRootElement()->AddChild(pRightBar2Bg);
	pGui->GetRootElement()->AddChild(pLeftHpBar);
	pGui->GetRootElement()->AddChild(pRightHpBar);
	pGui->GetRootElement()->AddChild(pLeftEnergyBar);
	pGui->GetRootElement()->AddChild(pRightEnergyBar);
	pGui->GetRootElement()->AddChild(pLeftEnergyIcon);
	pGui->GetRootElement()->AddChild(pRightEnergyIcon);
	pGui->GetRootElement()->AddChild(pLeftHealthIcon);
	pGui->GetRootElement()->AddChild(pRightHealthIcon);

	pScore = PNEW(CGuiTextbox);
	pScore->SetAlign(EHA_Center, EVA_Center);
	pGui->GetRootElement()->AddChild(pScore);

	pFPS = PNEW(CGuiText);
	pFPS->SetFont("consola.ttf", 12);
	pFPS->SetAlign(EHA_Left, EVA_Center);
	pFPS->SetColor(Color(255, 255, 255, 0));
	pFPS->SetSize(IntVec(300, 25));
	pFPS->SetPosition(IntVec(0, 0));
	pGui->GetRootElement()->AddChild(pFPS);

	pMenu = PNEW(CGuiElement);
	pGui->GetRootElement()->AddChild(pMenu);

	pMenuBg = PNEW(CGuiImage);
	pMenuBg->SetImage("resources/bg.png");
	pMenuBg->SetPosition(IntVec(0, 0));
	pMenu->AddChild(pMenuBg);
	
	CTexture::LoadTexture("resources/title.png");
	CTexture::LoadTexture("resources/button.png");

	pTitleImg = PNEW(CGuiImage);
	pTitleImg->SetImage("resources/title.png");
	pTitleImg->SetSize(IntVec(512,256)*2);
	pTitleImg->SetPosition(IntVec(0, 0));
	pMenu->AddChild(pTitleImg);

	{
		pPlayerText1 = PNEW(CGuiTextbox);   
		STextBlock b1;
		b1.SetFont("consolab.ttf");
		b1.SetFontSize(32);
		b1.SetColor(Color(100, 255, 100, 0));
		b1.SetText("Player 1\n");

		STextBlock b2;
		b2.SetFont("consolab.ttf");
		b2.SetFontSize(24);
		b2.SetColor(Color(255, 255, 255, 0));
		b2.SetText("Move: WSAD\nFire: Space");

		pPlayerText1->AddBlock(b1);
		pPlayerText1->AddBlock(b2);
		pPlayerText1->SetAlign(EHA_Center, EVA_Center);
		pPlayerText1->SetSize(IntVec(300, 200));
		pPlayerText1->SetPosition(IntVec(50, 300));
		pMenu->AddChild(pPlayerText1);
	}

	{
		pPlayerText2 = PNEW(CGuiTextbox);
		STextBlock b1;
		b1.SetFont("consolab.ttf");
		b1.SetFontSize(32);
		b1.SetColor(Color(0x3ab1ff00));
		b1.SetText("Player 2\n");

		STextBlock b2;
		b2.SetFont("consolab.ttf");
		b2.SetFontSize(24);
		b2.SetColor(Color(255, 255, 255, 0));
		b2.SetText("Move: Arrows\nFire: Enter");

		pPlayerText2->AddBlock(b1);
		pPlayerText2->AddBlock(b2);
		pPlayerText2->SetAlign(EHA_Center, EVA_Center);
		pPlayerText2->SetSize(IntVec(300, 200));
		pPlayerText2->SetPosition(IntVec(650, 300));
		pMenu->AddChild(pPlayerText2);
	}

	pStartButton = PNEW(CGuiImage);
	pStartButton->SetImage("resources/button.png");
	pStartButton->SetSize(IntVec(256, 128));
	pMenu->AddChild(pStartButton);

	pStartButton->AddClickEventListener([=](SClickEvent& Event) {
		pMenu->SetVisible(false);
		ResetGame();
	});

	pStart = PNEW(CGuiText);
	pStart->SetText("Start");
	pStart->SetFont("consolab.ttf",32);
	pStart->SetSize(IntVec(256, 128));
	pStart->SetAlign(EHA_Center, EVA_Center);
	pStart->SetColor(Color(255, 255, 255, 0));
	pStartButton->AddChild(pStart);


	pGameOverBg = PNEW(CGuiImage);
	pGameOverBg->SetImage("resources/black.png");
	pGameOverBg->SetVisible(false);
	pGui->GetRootElement()->AddChild(pGameOverBg);

	pOverviewMapImage = PNEW(CGuiImage);
	pGameOverBg->AddChild(pOverviewMapImage);

	pGameOverText = PNEW(CGuiTextbox);
	pGameOverText->SetAlign(EHA_Center, EVA_Center);
	pGameOverBg->AddChild(pGameOverText);

	pGameOverBg->AddClickEventListener([=](SClickEvent& Event) {
		pGameOverBg->SetVisible(false);
		pMenu->SetVisible(true);
	});

	return true;
}

void CRatGame::ResetGame()
{
	nRound = 0;
	scores[0] = 0;
	scores[1] = 0;
	UpdateScore();


	for (int y = 0; y < MAP_SIZE_Y; ++y)
	{
		for (int x = 0; x < MAP_SIZE_X; ++x)
		{
			mapdata[y][x] = (ETile)(random_4(e1) + ET_Dirt1);
		}
	}



	//top
	{
		float offset = random_float(e1) / 6.28f;
		for (int x = 0; x < MAP_SIZE_X; ++x)
		{
			int size = (int)(CIKK_CAKK_AMP + CIKK_CAKK2_AMP + sinf(offset + x * CIKK_CAKK_FREQ) * CIKK_CAKK_AMP + sinf(offset + x * CIKK_CAKK2_FREQ) * CIKK_CAKK2_AMP);
			for (int y = 0; y < size; ++y)
			{
				mapdata[y][x] = ET_Rock;
			}
		}
	}

	//bottom
	{
		float offset = random_float(e1) / 6.28f;
		for (int x = 0; x < MAP_SIZE_X; ++x)
		{
			int size = (int)(CIKK_CAKK_AMP + CIKK_CAKK2_AMP + sinf(offset + x * CIKK_CAKK_FREQ) * CIKK_CAKK_AMP + sinf(offset + x * CIKK_CAKK2_FREQ) * CIKK_CAKK2_AMP);
			for (int y = 0; y < size; ++y)
			{
				mapdata[MAP_SIZE_Y - 1 - y][x] = ET_Rock;
			}
		}
	}

	//left
	{
		float offset = random_float(e1) / 6.28f;
		for (int y = 0; y < MAP_SIZE_Y; ++y)
		{
			int size = (int)(CIKK_CAKK_AMP + CIKK_CAKK2_AMP + sinf(offset + y * CIKK_CAKK_FREQ) * CIKK_CAKK_AMP + sinf(offset + y * CIKK_CAKK2_FREQ) * CIKK_CAKK2_AMP);
			for (int x = 0; x < size; ++x)
			{
				mapdata[y][x] = ET_Rock;
			}
		}
	}

	//right
	{
		float offset = random_float(e1) / 6.28f;
		for (int y = 0; y < MAP_SIZE_Y; ++y)
		{
			int size = (int)(CIKK_CAKK_AMP + CIKK_CAKK2_AMP + sinf(offset + y * CIKK_CAKK_FREQ) * CIKK_CAKK_AMP + sinf(offset + y * CIKK_CAKK2_FREQ) * CIKK_CAKK2_AMP);
			for (int x = 0; x < size; ++x)
			{
				mapdata[y][MAP_SIZE_X - 1 - x] = ET_Rock;
			}
		}
	}

	if (!m_base1.expired())
	{
		m_base1.lock()->DestroySelf();
	}
	if (!m_base2.expired())
	{
		m_base2.lock()->DestroySelf();
	}
	if (!m_recharge1.expired())
	{
		m_recharge1.lock()->DestroySelf();
	}
	if (!m_recharge2.expired())
	{
		m_recharge2.lock()->DestroySelf();
	}


	auto base1 = std::make_shared<CBase>();
	auto base2 = std::make_shared<CBase>();
	auto recharge1 = std::make_shared<CRechargeArea>();
	auto recharge2 = std::make_shared<CRechargeArea>();

	m_base1 = base1;
	m_base2 = base2;
	m_recharge1 = recharge1;
	m_recharge2 = recharge2;

	//bases
	IntVec pos = { CIKK_CAKK_AMP * 2, CIKK_CAKK_AMP * 2 };
	IntVec size = { MAP_SIZE_X - CIKK_CAKK_AMP * 4 - BASE_SIZE_X, MAP_SIZE_Y - CIKK_CAKK_AMP * 4 - BASE_SIZE_Y };
	{
		IntVec size2 = { size.x / 2 - CIKK_CAKK_AMP * 2, size.y };
		IntVec basepos = IntVec(pos.x + size2.x * (float)(random_float(e1)), pos.y + size2.y * (float)(random_float(e1)));
		m_vSpawnPos1 = basepos + IntVec(BASE_SIZE_X / 2, BASE_SIZE_Y / 2);
		for (int x = 0; x < BASE_SIZE_X; ++x)
		{
			for (int y = 0; y < BASE_SIZE_Y; ++y)
			{
				SetTile(basepos + IntVec(x, y), ET_Empty);
			}
		}
		root->AttachGameObject(base1, (basepos - IntVec(1, 6)) * TILE_SIZE);
		root->AttachGameObject(recharge1, (basepos - IntVec(1, 6)) * TILE_SIZE);
	}
	{
		IntVec size2 = { size.x / 2 - CIKK_CAKK_AMP * 2, size.y };
		IntVec pos2 = { pos.x + size.x / 2 + CIKK_CAKK_AMP * 2 ,pos.y };
		IntVec basepos = IntVec(pos2.x + size2.x * (float)(random_float(e1)), pos2.y + size2.y * (float)(random_float(e1)));
		m_vSpawnPos2 = basepos + IntVec(BASE_SIZE_X / 2, BASE_SIZE_Y / 2);
		for (int x = 0; x < BASE_SIZE_X; ++x)
		{
			for (int y = 0; y < BASE_SIZE_Y; ++y)
			{
				SetTile(basepos + IntVec(x, y), ET_Empty);
			}
		}
		root->AttachGameObject(base2, (basepos - IntVec(1, 6)) * TILE_SIZE);
		root->AttachGameObject(recharge2, (basepos - IntVec(1, 6)) * TILE_SIZE);
	}



	if (random_bool(e1) == 0)
	{
		IntVec temp = m_vSpawnPos1;
		m_vSpawnPos1 = m_vSpawnPos2;
		m_vSpawnPos2 = temp;
		base1->SetPlayer(1);
		base2->SetPlayer(0);
		recharge1->m_nPlayer = 1;
		recharge2->m_nPlayer = 0;
	}
	else
	{
		base1->SetPlayer(0);
		base2->SetPlayer(1);
		recharge1->m_nPlayer = 0;
		recharge2->m_nPlayer = 1;
	}


	SpawnRats();
}


void CRatGame::Resize()
{
	IntVec vWinSize = SDLManager::GetSize();

	pBgCamera1->SetRect(IntRect(0, 0, vWinSize.x / 2, vWinSize.y*0.85));
	pBgCamera2->SetRect(IntRect(vWinSize.x / 2, 0, vWinSize.x / 2, vWinSize.y*0.85));
	m_DirtLayer1.pDirtLayer->SetRect(IntRect(0, 0, vWinSize.x / 2, vWinSize.y*0.85));
	m_DirtLayer2.pDirtLayer->SetRect(IntRect(vWinSize.x / 2, 0, vWinSize.x / 2, vWinSize.y*0.85));
	pCamera1->SetRect(IntRect(0, 0, vWinSize.x / 2, vWinSize.y*0.85));
	pCamera2->SetRect(IntRect(vWinSize.x / 2, 0, vWinSize.x / 2, vWinSize.y*0.85));
	pGui->SetRect(IntRect(0, 0, vWinSize.x, vWinSize.y));
	pMenuBg->SetSize(vWinSize);
	pTitleImg->SetPosition(IntVec((vWinSize.x - pTitleImg->GetSize().x)/2, 0));

	pPlayerText1->SetSize(IntVec(vWinSize.x/2, vWinSize.y/3));
	pPlayerText1->SetPosition(IntVec(0, vWinSize.y /3*2));
	pPlayerText2->SetSize(IntVec(vWinSize.x / 2, vWinSize.y / 3));
	pPlayerText2->SetPosition(IntVec(vWinSize.x / 2, vWinSize.y / 3 * 2));

	pStartButton->SetPosition(IntVec((vWinSize.x - pStartButton->GetSize().x) / 2, vWinSize.y - 250));
	pStart->SetPosition(IntVec((vWinSize.x - pStartButton->GetSize().x) / 2, vWinSize.y - 250));

	pBg[0]->SetSize(Vec(vWinSize.x, vWinSize.y*0.02f));
	pBg[0]->SetPosition(Vec(0, 0));
	pBg[1]->SetSize(Vec(vWinSize.x*0.01f, vWinSize.y));
	pBg[1]->SetPosition(Vec(0, 0));
	pBg[2]->SetSize(Vec(vWinSize.x*0.03f, vWinSize.y));
	pBg[2]->SetPosition(Vec(vWinSize.x*0.49, 0));
	pBg[3]->SetSize(Vec(vWinSize.x*0.01, vWinSize.y));
	pBg[3]->SetPosition(Vec(vWinSize.x*0.99f, 0));
	pBg[4]->SetSize(Vec(vWinSize.x, vWinSize.y*0.17f));
	pBg[4]->SetPosition(Vec(0, vWinSize.y*0.83f));

	pGameOverBg->SetSize(vWinSize);
	pGameOverText->SetSize(vWinSize * Vec(1.0f, 0.2f));

	float scale = vWinSize.y * 0.5f / MAP_SIZE_Y;

	pOverviewMapImage->SetSize(IntVec(MAP_SIZE_X * scale,MAP_SIZE_Y * scale));
	pOverviewMapImage->SetPosition(IntVec((vWinSize.x - MAP_SIZE_X * scale)/2, vWinSize.y*0.25));

	m_bResized = false;
}

void CRatGame::Update()
{
	root->UpdateInternal(Vec(0, 0));

	if (m_bResized)
	{
		Resize();
	}

	auto rat1 = m_rat1.lock();
	if (rat1)
	{
		IntVec CameraPos = rat1->GetAbsolutePos() - (pCamera1->GetRect().GetSize() / 2);
		pCamera1->SetPosition(CameraPos);
		pBgCamera1->SetPosition(CameraPos);
	}

	m_DirtLayer1.Update(pCamera1->GetPosition());

	auto rat2 = m_rat2.lock();
	if (rat2)
	{
		IntVec CameraPos = rat2->GetAbsolutePos() - (pCamera2->GetRect().GetSize() / 2);
		pCamera2->SetPosition(CameraPos);
		pBgCamera2->SetPosition(CameraPos);
	}

	m_DirtLayer2.Update(pCamera2->GetPosition());

	UpdateGui();

	if (roundrestarttimer > 0)
	{
		roundrestarttimer -= Time::frame;
		if (roundrestarttimer <= 0)
		{
			roundrestarttimer = -1;

			if (scores[0] == 3 || scores[1] == 3)
			{
				GameOver();
			}
			else
			{
				SpawnRats();
				UpdateScore();
			}
			
		}

	}

	static float explosiontimes[10] = { 0.0f, 0.05f, 0.1f, 0.15f, 0.2f, 0.25f, 0.30f, 0.35f, 0.40f, 0.45f };
	static int explosionsizes[10] = { 20,40,60,80,100,120,140,160,230,300 };
	static int explosionnums[10] = { 10,20,30,40,50,60,70,70,60,10 };

	if (explosionTimer != -1)
	{

		double newtime = explosionTimer + Time::frame;
		for (int i = 0; i < 10; ++i)
		{
			if (explosionTimer <= explosiontimes[i] && newtime > explosiontimes[i])
			{
				Explode(explosionsizes[i], explosionnums[i]);
			}
		}
		explosionTimer = newtime;
	}

	pFPS->SetText("fps:" + std::to_string(Time::fps) + " sprites:" + std::to_string(SDLManager::drawnum));

	if (Input::GetKey(KEY_R).pressed)
	{
		GameOver();
	}

	if (Input::GetKey(KEY_K).pressed)
	{
		m_rat1.lock()->Die();
	}

	if (Input::GetKey(KEY_RETURN).pressed)
	{
		if (pMenu->IsVisible())
		{
			pMenu->SetVisible(false);
			ResetGame();
		}
		if (pGameOverBg->IsVisible())
		{
			pGameOverBg->SetVisible(false);
			pMenu->SetVisible(true);
		}
	}
}

void CRatGame::GameOver()
{

	pMapImage->Fill([=](int y, int x) -> int {

		ETile tile = mapdata[x][y];
		switch (tile)
		{
		case ET_Empty: return 0x463428ff;
		case ET_Dirt1:
		case ET_Dirt2:
		case ET_Dirt3:
		case ET_Dirt4: return 0xeba456ff;
		case ET_Rock: return 0x7d7d7dff;
		default:
			return 0x00000000;
		}

	});

	CTexture::DelTexture("mapimage");
	pMapImage->CreateTexture("mapimage");
	pOverviewMapImage->SetImage("mapimage");
	pGameOverBg->SetVisible(true);

	STextBlock text;
	text.SetFont("consolab.ttf");
	text.SetFontSize(36);
	text.SetColor(scores[0] > scores[1] ? Color(100, 255, 100, 0) : Color(0x3ab1ff00));
	text.SetText((scores[0] > scores[1]) ? "Player 1" : "Player 2");

	STextBlock text2;
	text2.SetFont("consolab.ttf");
	text2.SetFontSize(36);
	text2.SetColor(Color(255, 255, 255, 255));
	text2.SetText(" wins!");

	pGameOverText->Clear();
	pGameOverText->AddBlock(text);
	pGameOverText->AddBlock(text2);
}

void CRatGame::SpawnRats()
{
	if (!m_rat1.expired())
	{
		m_rat1.lock()->DestroySelf();
	}
	if (!m_rat2.expired())
	{
		m_rat2.lock()->DestroySelf();
	}

	std::shared_ptr<CRat> rat1;
	std::shared_ptr<CRat> rat2;
	rat1 = std::make_shared<CRat>();
	rat1->m_nPlayer = 0;
	root->AttachGameObject(rat1, Vec(0, 0));
	m_rat1 = rat1;

	rat2 = std::make_shared<CRat>();
	rat2->m_nPlayer = 1;
	root->AttachGameObject(rat2, Vec(0, 0));
	m_rat2 = rat2;
	rat1->SetPosition(m_vSpawnPos1 * TILE_SIZE);
	rat2->SetPosition(m_vSpawnPos2 * TILE_SIZE);
}

void CRatGame::UpdateGui()
{
	IntVec vWinSize = SDLManager::GetSize();

	float iconsize = vWinSize.y*0.03f;
	float offset = iconsize * 1.5f;

	for (const PGuiImage& pImg : { pLeftBar1Bg ,pLeftBar2Bg ,pRightBar1Bg ,pRightBar2Bg })
	{
		pImg->SetSize(IntVec((int)(vWinSize.x*0.3f - offset), (int)(vWinSize.y*0.03f)));
	}

	auto rat1 = m_rat1.lock();
	auto rat2 = m_rat2.lock();
	if (rat1)
	{
		fHp1 = rat1->GetHealthRate();
		fEnergy1 = rat1->GetEnergyRate();
	}
	else
	{
		fHp1 = 0;
	}

	if (rat2)
	{
		fHp2 = rat2->GetHealthRate();
		fEnergy2 = rat2->GetEnergyRate();
	}
	else
	{
		fHp2 = 0;
	}

	pLeftHpBar->SetSize(IntVec((int)((vWinSize.x*0.3f - offset) *fHp1), (int)(vWinSize.y*0.03f)));
	pLeftEnergyBar->SetSize(IntVec((int)((vWinSize.x*0.3f - offset)*fEnergy1), (int)(vWinSize.y*0.03f)));
	pRightHpBar->SetSize(IntVec((int)((vWinSize.x*0.3f - offset)*fHp2), (int)(vWinSize.y*0.03f)));
	pRightEnergyBar->SetSize(IntVec((int)((vWinSize.x*0.3f - offset)*fEnergy2), (int)(vWinSize.y*0.03f)));


	IntVec l1((int)(vWinSize.x*0.1f + offset), (int)(vWinSize.y*0.88f));
	IntVec l2((int)(vWinSize.x*0.1f + offset), (int)(vWinSize.y*0.94f));
	IntVec r1((int)(vWinSize.x*0.6f + offset), (int)(vWinSize.y*0.88f));
	IntVec r2((int)(vWinSize.x*0.6f + offset), (int)(vWinSize.y*0.94f));

	pLeftBar1Bg->SetPosition(l1);
	pLeftHpBar->SetPosition(l1);
	pLeftBar2Bg->SetPosition(l2);
	pLeftEnergyBar->SetPosition(l2);

	pRightBar1Bg->SetPosition(r1);
	pRightHpBar->SetPosition(r1);
	pRightBar2Bg->SetPosition(r2);
	pRightEnergyBar->SetPosition(r2);

	for (const PGuiImage& pImg : { pLeftEnergyIcon ,pLeftHealthIcon, pRightEnergyIcon, pRightHealthIcon })
	{
		pImg->SetSize(IntVec((int)(iconsize), (int)(iconsize)));
	}


	pLeftEnergyIcon->SetPosition(IntVec((int)(vWinSize.x*0.1f), (int)(vWinSize.y*0.94f)));
	pLeftHealthIcon->SetPosition(IntVec((int)(vWinSize.x*0.1f), (int)(vWinSize.y*0.88f)));
	pRightEnergyIcon->SetPosition(IntVec((int)(vWinSize.x*0.6f), (int)(vWinSize.y*0.94f)));
	pRightHealthIcon->SetPosition(IntVec((int)(vWinSize.x*0.6f), (int)(vWinSize.y*0.88f)));

	pScore->SetSize(IntVec((int)vWinSize.x*0.2f,(int)(vWinSize.y*0.09)));
	pScore->SetPosition(IntVec((int)vWinSize.x*0.4f, (vWinSize.y*0.88)));

	bool bShow = (Time::full*2 - int(Time::full*2)) < 0.5;

	pLeftEnergyBar->SetVisible(fEnergy1 < 0.15 ? bShow : true);
	pRightEnergyBar->SetVisible(fEnergy2 < 0.15 ? bShow : true);

}

void CRatGame::OnResize()
{
	m_bResized = true;
}

void CRatGame::AddScore(int nPlayer)
{
	scores[nPlayer]++;
	UpdateScore();
	roundrestarttimer = 5;
	nRound++;
}


void CRatGame::UpdateScore()
{
	pScore->Clear();

	STextBlock round;
	round.SetFont("consolab.ttf");
	round.SetFontSize(36);
	round.SetColor(Color(255, 255, 255, 0));
	round.SetText("Round " + std::to_string(nRound+1) + "\n");
	pScore->AddBlock(round);

	STextBlock scoretext;
	scoretext.SetFont("consolab.ttf");
	scoretext.SetFontSize(48);
	scoretext.SetColor(Color(255, 255, 255, 0));
	scoretext.SetText(std::to_string(scores[0]) + ":" + std::to_string(scores[1]));
	pScore->AddBlock(scoretext);
}

void CRatGame::StartExplosion(IntVec pos)
{
	vExplosion = pos;
	explosionTimer = 0;
}

void CRatGame::Explode(float strength, int count)
{
	for (int i = 0; i < count; ++i)
	{
		Vec dir = Vec::FromAngle(random_float(e1) * PI * 2);
		int nLen = (int)(random_float(e1)*strength*0.25f + strength * 0.75f);
		
		for (int k = 0; k < nLen; ++k)
		{
			BreakTileAtPos(vExplosion + dir * k);
		}

	}
}


void CDirtLayer::Load()
{
	CTexture::LoadTexture("resources/dirt0.png");
	CTexture::LoadTexture("resources/dirt1.png");
	CTexture::LoadTexture("resources/dirt2.png");
	CTexture::LoadTexture("resources/dirt3.png");
	CTexture::LoadTexture("resources/rock.png");
	pDirt1 = CTexture::GetTexture("resources/dirt0.png");
	pDirt2 = CTexture::GetTexture("resources/dirt1.png");
	pDirt3 = CTexture::GetTexture("resources/dirt2.png");
	pDirt4 = CTexture::GetTexture("resources/dirt3.png");
	pRock = CTexture::GetTexture("resources/rock.png");
}

void CDirtLayer::Update(IntVec vPos)
{
	IntRect Rect = pDirtLayer->GetRect();
	IntVec Size((int)ceilf(Rect.GetSize().x / (float)TILE_SIZE), (int)ceilf(Rect.GetSize().y / (float)TILE_SIZE));

	m_aDirtSprites.resize(Size.x*Size.y);

	IntVec vMapOffset = vPos / TILE_SIZE;
	vMapOffset += IntVec(1, 1);
	IntVec vOffset = ((vPos / TILE_SIZE) * TILE_SIZE) - vPos;
	Vec vOffset2 = vOffset + pDirtLayer->GetRect().GetUpperLeft(); 
	for (int y = 0; y < Size.y; ++y)
	{
		for (int x = 0; x < Size.x; ++x)
		{
			int idx = y * Size.x + x;
			std::shared_ptr<CImageSprite> pSprite = m_aDirtSprites[idx];
			if (!pSprite)
			{
				pSprite = std::make_shared<CImageSprite>();
				m_aDirtSprites[idx] = pSprite;
				pDirtLayer->AddSprite(pSprite);
			}

			pSprite->SetPos(vOffset2 + Vec(TILE_SIZE * x, TILE_SIZE * y));
			pSprite->SetSize(Vec(TILE_SIZE, TILE_SIZE));

			ETile tile = CRatGame::GetInstance()->GetTile(IntVec(x, y) + vMapOffset);
			switch (tile)
			{
			case ET_Empty:
				pSprite->SetTexture(pEmpty);
				break;
			case ET_Dirt1:
				pSprite->SetTexture(pDirt1);
				break;
			case ET_Dirt2:
				pSprite->SetTexture(pDirt2);
				break;
			case ET_Dirt3:
				pSprite->SetTexture(pDirt3);
				break;
			case ET_Dirt4:
				pSprite->SetTexture(pDirt4);
				break;
			case ET_Rock:
				pSprite->SetTexture(pRock);
				break;
			default:
				break;
			}			
		}
	}
}

void CDirtLayer::OnResize()
{
	pDirtLayer->ClearSprites();
	m_aDirtSprites.clear();
}