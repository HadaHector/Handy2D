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

CRatGame* CRatGame::m_pInstance;


void CProjectile::Init()
{
	CTexture::LoadTexture("resources/projectile.png");
	std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
	pSprite->SetTexture(CTexture::GetTexture("resources/projectile.png"));
	pSprite->SetSize(8, 8);
	AttachSprite(pSprite, Vec(-4, -4), { "camera" });
	AddCollider(IntRect(-4, -4, 8, 8), true, OVERLAP | OVERLAP_EVENTS);
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
		if (rand() % 4 > 0)
		{
			CRatGame::GetInstance()->BreakTile(coord + dir);
			if (rand() % 3 == 0)
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
		DestroySelf();
		return true;
	}
	if (tile >= ET_Dirt1 && tile <= ET_Dirt4)
	{
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
	m_pRat = std::make_shared<CAnimSprite>();
	m_pRat->SetTexture(CTexture::GetTexture("resources/rat_left.png"));
	m_pRat->SetRowsAndCols(2, 2);
	m_pRat->SetSize(48, 48);
	AttachSprite(m_pRat, Vec(-24, -24), { "camera" });
	AddCollider(IntRect(-24, -24, 48, 48), true, OVERLAP | OVERLAP_EVENTS | COLLIDE | COLLIDE_EVENTS);
	AddSocket("shoot", Vec(0, 0));
}

void CRat::Fire()
{
	auto bullet = std::make_shared<CProjectile>();
	bullet->SetSpeed(m_vDir * 1000);
	bullet->SetPlayer(m_nPlayer);
	GetParent()->AttachGameObject(bullet, GetSocketAbsPos("shoot"));
}

void CRat::Update()
{
	bool bMoving = false;
	if (m_freezetimer < Time::frame)
	{
		double time = Time::frame - m_freezetimer;		
		bool bLeft = Input::GetKey(m_nPlayer ? KEY_KP_4 : KEY_A).active;
		bool bRight = Input::GetKey(m_nPlayer ? KEY_KP_6 : KEY_D).active;
		bool bUp = Input::GetKey(m_nPlayer ? KEY_KP_8 : KEY_W).active;
		bool bDown = Input::GetKey(m_nPlayer ? KEY_KP_5 : KEY_S).active;

		std::string tex;
		if (bUp)
		{
			if (bLeft)
			{
				m_vDir = { -1,-1 };
				tex = "resources/rat_upleft.png";
			}
			else if (bRight)
			{
				m_vDir = { 1,-1 };
				tex = "resources/rat_upright.png";
			}
			else
			{
				m_vDir = { 0,-1 };
				tex = "resources/rat_up.png";
			}
		}
		else if (bDown)
		{
			if (bLeft)
			{
				m_vDir = { -1,1 };
				tex = "resources/rat_downleft.png";
			}
			else if (bRight)
			{
				m_vDir = { 1,1 };
				tex = "resources/rat_downright.png";
			}
			else
			{
				m_vDir = { 0,1 };
				tex = "resources/rat_down.png";
			}
		}
		else if (bLeft)
		{
			m_vDir = { -1,0 };
			tex = "resources/rat_left.png";
		}
		else if (bRight)
		{
			m_vDir = { 1,0 };
			tex = "resources/rat_right.png";
		}

		if (tex.size() > 0)
		{
			m_vDir.Normalize();
			AddMovement(m_vDir * Time::frame * 300.0f);
			m_pRat->SetTexture(CTexture::GetTexture(tex));
			m_pRat->SetSize(48, 48);
			bMoving = true;
		}
	}

	m_freezetimer = std::max(0.0f, (float)( m_freezetimer - Time::frame));

	if (Input::GetKey(m_nPlayer ? KEY_KP_ENTER : KEY_SPACE).active && m_cooldown == 0)
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
		m_nStep = (m_nStep + 1) % 4;
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
	DestroySelf();
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
	m_DirtLayer1.Load();
	m_DirtLayer2.Load();
	pCamera1 = new CCameraRenderLayer(IntRect(0, 0, 512, 512));
	pCamera2 = new CCameraRenderLayer(IntRect(512, 0, 512, 512));
	pBgCamera1 = new CCameraRenderLayer(IntRect(0, 0, 512, 512));
	pBgCamera2 = new CCameraRenderLayer(IntRect(512, 0, 512, 512));
	m_DirtLayer1.pDirtLayer = new CSpriteRenderLayer(IntRect(0, 0, 512, 512));
	m_DirtLayer2.pDirtLayer = new CSpriteRenderLayer(IntRect(512, 0, 512, 512));
	pCamera1->AddTag("camera");
	pCamera2->AddTag("camera");
	pCamera2->SetPosition(IntVec(0, 0));
	SDLManager::Instance.AddLayer(pBgCamera1);
	SDLManager::Instance.AddLayer(pBgCamera2);
	SDLManager::Instance.AddLayer(m_DirtLayer1.pDirtLayer);
	SDLManager::Instance.AddLayer(m_DirtLayer2.pDirtLayer);
	SDLManager::Instance.AddLayer(pCamera1);
	SDLManager::Instance.AddLayer(pCamera2);

	CTexture::LoadTexture("resources/bg.png");
	pBg = std::make_shared<CImageSprite>();
	pBg->SetTexture(CTexture::GetTexture("resources/bg.png"));
	pBg->SetSize(2048, 2048);
	pBgCamera1->AddSprite(pBg);
	pBgCamera2->AddSprite(pBg);

	root = std::make_shared<CGameObject>();

	Vec Size = SDLManager::Instance.GetSize();

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


	for (int y = 0; y < MAP_SIZE_Y; ++y)
	{
		for (int x = 0; x < MAP_SIZE_X; ++x)
		{
			mapdata[y][x] = (ETile)(rand() % 4 + ET_Dirt1);
		}
	}

	//top
	{
		float offset = rand() % 1000 / 6.28f;
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
		float offset = rand() % 1000 / 6.28f;
		for (int x = 0; x < MAP_SIZE_X; ++x)
		{
			int size = (int)(CIKK_CAKK_AMP + CIKK_CAKK2_AMP + sinf(offset + x * CIKK_CAKK_FREQ) * CIKK_CAKK_AMP + sinf(offset + x * CIKK_CAKK2_FREQ) * CIKK_CAKK2_AMP);
			for (int y = 0; y < size; ++y)
			{
				mapdata[MAP_SIZE_Y-1-y][x] = ET_Rock;
			}
		}
	}

	//left
	{
		float offset = rand() % 1000 / 6.28f;
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
		float offset = rand() % 1000 / 6.28f;
		for (int y = 0; y < MAP_SIZE_Y; ++y)
		{
			int size = (int)(CIKK_CAKK_AMP + CIKK_CAKK2_AMP + sinf(offset + y * CIKK_CAKK_FREQ) * CIKK_CAKK_AMP + sinf(offset + y * CIKK_CAKK2_FREQ) * CIKK_CAKK2_AMP);
			for (int x = 0; x < size; ++x)
			{
				mapdata[y][MAP_SIZE_X-1-x] = ET_Rock;
			}
		}
	}

	//bases
	IntVec pos = { CIKK_CAKK_AMP * 2, CIKK_CAKK_AMP * 2 };
	IntVec size = { MAP_SIZE_X - CIKK_CAKK_AMP * 4 - BASE_SIZE, MAP_SIZE_Y - CIKK_CAKK_AMP * 4 - BASE_SIZE };
	{
		IntVec size2 = { size.x / 2 - CIKK_CAKK_AMP * 2, size.y };
		IntVec basepos = IntVec(pos.x + size2.x * (float)(rand() % 1000 / 1000.0f), pos.y + size2.y * (float)(rand() % 1000 / 1000.0f) );
		m_vSpawnPos1 = basepos + IntVec(BASE_SIZE / 2, BASE_SIZE / 2);
		for (int x = 0; x < BASE_SIZE; ++x)
		{
			for (int y = 0; y < BASE_SIZE; ++y)
			{
				SetTile(basepos + IntVec(x, y), ET_Empty);
			}
		}
	}
	{
		IntVec size2 = { size.x / 2 - CIKK_CAKK_AMP * 2, size.y };
		IntVec pos2 = {pos.x + size.x / 2 + CIKK_CAKK_AMP * 2 ,pos.y};
		IntVec basepos = IntVec(pos2.x + size2.x * (float)(rand() % 1000 / 1000.0f), pos2.y + size2.y * (float)(rand() % 1000 / 1000.0f));
		m_vSpawnPos2 = basepos + IntVec(BASE_SIZE / 2, BASE_SIZE / 2);
		for (int x = 0; x < BASE_SIZE; ++x)
		{
			for (int y = 0; y < BASE_SIZE; ++y)
			{
				SetTile(basepos + IntVec(x, y), ET_Empty);
			}
		}
	}

	if (rand() % 2 == 0)
	{
		IntVec temp = m_vSpawnPos1;
		m_vSpawnPos1 = m_vSpawnPos2;
		m_vSpawnPos2 = temp;
	}

	rat1->SetPosition(m_vSpawnPos1 * TILE_SIZE);
	rat2->SetPosition(m_vSpawnPos2 * TILE_SIZE);



	pGui = new CGuiLayer(IntRect(0, 0, 1024, 512));
	SDLManager::Instance.AddLayer(pGui);


	CTexture::LoadTexture("resources/barbg.png");
	CTexture::LoadTexture("resources/hpbar.png");
	CTexture::LoadTexture("resources/energybar.png");



	pLeftBar1Bg = new CGuiImage();
	pLeftBar1Bg->SetImage("resources/barbg.png");
	pLeftBar2Bg = new CGuiImage();
	pLeftBar2Bg->SetImage("resources/barbg.png");
	pRightBar1Bg = new CGuiImage();
	pRightBar1Bg->SetImage("resources/barbg.png");
	pRightBar2Bg = new CGuiImage();
	pRightBar2Bg->SetImage("resources/barbg.png");

	pLeftHpBar = new CGuiImage();
	pLeftHpBar->SetImage("resources/hpbar.png");
	pRightHpBar = new CGuiImage();
	pRightHpBar->SetImage("resources/hpbar.png");
	pLeftEnergyBar = new CGuiImage();
	pLeftEnergyBar->SetImage("resources/energybar.png");
	pRightEnergyBar = new CGuiImage();
	pRightEnergyBar->SetImage("resources/energybar.png");

	pGui->GetRootElement()->AddChild(pLeftBar1Bg);
	pGui->GetRootElement()->AddChild(pLeftBar2Bg);
	pGui->GetRootElement()->AddChild(pRightBar1Bg);
	pGui->GetRootElement()->AddChild(pRightBar2Bg);
	pGui->GetRootElement()->AddChild(pLeftHpBar);
	pGui->GetRootElement()->AddChild(pRightHpBar);
	pGui->GetRootElement()->AddChild(pLeftEnergyBar);
	pGui->GetRootElement()->AddChild(pRightEnergyBar);


	return true;
}


void CRatGame::Resize()
{
	IntVec vWinSize = SDLManager::GetSize();

	pBgCamera1->SetRect(IntRect(0, 0, vWinSize.x / 2, vWinSize.y));
	pBgCamera2->SetRect(IntRect(vWinSize.x / 2, 0, vWinSize.x / 2, vWinSize.y));
	m_DirtLayer1.pDirtLayer->SetRect(IntRect(0, 0, vWinSize.x / 2, vWinSize.y));
	m_DirtLayer2.pDirtLayer->SetRect(IntRect(vWinSize.x / 2, 0, vWinSize.x / 2, vWinSize.y));
	pCamera1->SetRect(IntRect(0, 0, vWinSize.x / 2, vWinSize.y));
	pCamera2->SetRect(IntRect(vWinSize.x / 2, 0, vWinSize.x / 2, vWinSize.y));
	pGui->SetRect(IntRect(0, 0, vWinSize.x, vWinSize.y));

	m_DirtLayer1.OnResize();
	m_DirtLayer2.OnResize();

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
		m_DirtLayer1.Update(CameraPos);
		pCamera1->SetPosition(CameraPos);
		pBgCamera1->SetPosition(CameraPos);
	}

	auto rat2 = m_rat2.lock();
	if (rat2)
	{
		IntVec CameraPos = rat2->GetAbsolutePos() - (pCamera2->GetRect().GetSize() / 2);
		m_DirtLayer2.Update(CameraPos);
		pCamera2->SetPosition(CameraPos);
		pBgCamera2->SetPosition(CameraPos);
	}

	UpdateGui();
}

void CRatGame::UpdateGui()
{
	IntVec vWinSize = SDLManager::GetSize();

	for (CGuiImage* pImg : { pLeftBar1Bg ,pLeftBar2Bg ,pRightBar1Bg ,pRightBar2Bg })
	{
		pImg->SetSize(IntVec((int)(vWinSize.x*0.3f), (int)(vWinSize.y*0.05f)));
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

	pLeftHpBar->SetSize(IntVec((int)(vWinSize.x*0.3f*fHp1), (int)(vWinSize.y*0.05f)));
	pLeftEnergyBar->SetSize(IntVec((int)(vWinSize.x*0.3f*fEnergy1), (int)(vWinSize.y*0.05f)));
	pRightHpBar->SetSize(IntVec((int)(vWinSize.x*0.3f*fHp2), (int)(vWinSize.y*0.05f)));
	pRightEnergyBar->SetSize(IntVec((int)(vWinSize.x*0.3f*fEnergy2), (int)(vWinSize.y*0.05f)));


	IntVec l1((int)(vWinSize.x*0.1f), (int)(vWinSize.y*0.8f));
	IntVec l2((int)(vWinSize.x*0.1f), (int)(vWinSize.y*0.9f));
	IntVec r1((int)(vWinSize.x*0.6f), (int)(vWinSize.y*0.8f));
	IntVec r2((int)(vWinSize.x*0.6f), (int)(vWinSize.y*0.9f));

	pLeftBar1Bg->SetPosition(l1);
	pLeftHpBar->SetPosition(l1);
	pLeftBar2Bg->SetPosition(l2);
	pLeftEnergyBar->SetPosition(l2);

	pRightBar1Bg->SetPosition(r1);
	pRightHpBar->SetPosition(r1);
	pRightBar2Bg->SetPosition(r2);
	pRightEnergyBar->SetPosition(r2);
}

void CRatGame::OnResize()
{
	m_bResized = true;
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

			pSprite->SetPos(vOffset + Vec(TILE_SIZE * x, TILE_SIZE * y) + pDirtLayer->GetRect().GetUpperLeft());
			pSprite->SetSize(Vec(TILE_SIZE, TILE_SIZE));

			ETile tile = CRatGame::GetInstance()->GetTile(IntVec(x, y) + vMapOffset);
			switch (tile)
			{
			case ET_Empty:
				pSprite->SetTexture(std::weak_ptr<CTexture>());
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