#include "SDLManager.h"
#include "RenderLayer.h"
#include <array>
#include "Input.h"
#include <algorithm>
#include "GameObject.h"
#include "GuiLayer.h"
#include <random>
#include "CollisionManager.h"


class CDirt : public CGameObject
{
public:
	virtual void Init()
	{
		CTexture::LoadTexture("resources/dirt.png");
		std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
		pSprite->SetTexture(CTexture::GetTexture("resources/dirt.png"));
		pSprite->SetSize(64, 64);
		AttachSprite(pSprite, Vec(0, 0), { "camera" });
		AddCollider(IntRect(0, 0, 64, 64), false, OVERLAP | OVERLAP_EVENTS | COLLIDE);
	}

	virtual void Update()
	{
	}

};


class CProjectile : public CGameObject
{
	IntVec m_vSpeed;
	int m_nPlayer;
public:
	virtual void Init()
	{
		CTexture::LoadTexture("resources/projectile.png");
		std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
		pSprite->SetTexture(CTexture::GetTexture("resources/projectile.png"));
		pSprite->SetSize(8, 8);
		AttachSprite(pSprite, Vec(-4, -4), { "camera" });
		AddCollider(IntRect(-4, -4, 8, 8), true, OVERLAP | OVERLAP_EVENTS);
	}

	virtual void Update()
	{
		AddMovement(m_vSpeed * Time::frame);
	}

	void SetSpeed(const IntVec& spd)
	{
		m_vSpeed = spd;
	}

	void SetPlayer(int nPlayer)
	{
		m_nPlayer = nPlayer;
	}

	virtual void OnOverlap(const SOverlapEvent& event) override;

};


class CRat : public CGameObject
{
private:
	int m_nStep = 0;
	double m_timer = 0;
	double m_cooldown = 0;
	std::shared_ptr<CAnimSprite> m_pRat;

	const float m_fMaxHealth = 100;
	const float m_fMaxEnergy = 100;
	float m_fHealth = m_fMaxHealth;
	float m_fEnergy = m_fMaxEnergy;
public:

	float GetHealthRate() { return m_fHealth / m_fMaxHealth; }
	float GetEnergyRate() { return m_fEnergy / m_fMaxEnergy; }

	virtual void Init()
	{
		SetPosition(Vec(0, 0));
		CTexture::LoadTexture("resources/rat_left.png");
		CTexture::LoadTexture("resources/rat_right.png");
		m_pRat = std::make_shared<CAnimSprite>();
		m_pRat->SetTexture(CTexture::GetTexture("resources/rat_left.png"));
		m_pRat->SetRowsAndCols(4, 1);
		m_pRat->SetSize(170,64);
		AttachSprite(m_pRat, Vec(-85, -32), {"camera"});
		AddCollider(IntRect(-85, -32, 64, 64), true, OVERLAP | OVERLAP_EVENTS | COLLIDE | COLLIDE_EVENTS);
	}

	void Fire()
	{
		auto bullet = std::make_shared<CProjectile>();
		bullet->SetSpeed(IntVec(1000, 0));
		bullet->SetPlayer(m_nPlayer);
		GetParent()->AttachGameObject(bullet, GetAbsolutePos());
	}

	virtual void Update()
	{
		bool bMoving = false;
		if (Input::GetKey(m_nPlayer ? KEY_RIGHT : KEY_D).active)
		{
			AddMovement({ (float)Time::frame * 300.0f,0.0f});
			m_pRat->SetTexture(CTexture::GetTexture("resources/rat_right.png"));
			m_pRat->SetSize(170, 64);
			bMoving = true;
		}
		if (Input::GetKey(m_nPlayer ? KEY_LEFT : KEY_A).active)
		{
			AddMovement({ (float)Time::frame * -300.0f,0.0f });
			m_pRat->SetTexture(CTexture::GetTexture("resources/rat_left.png"));
			m_pRat->SetSize(170, 64);
			bMoving = true;
		}
		if (Input::GetKey(m_nPlayer ? KEY_DOWN : KEY_S).active)
		{
			AddMovement({ 0.0f,(float)Time::frame * 300.0f });
			bMoving = true;
		}
		if (Input::GetKey(m_nPlayer ? KEY_UP : KEY_W).active)
		{
			AddMovement({ 0.0f, (float)Time::frame * -300.0f });
			bMoving = true;
		}
		if (Input::GetKey(m_nPlayer ? KEY_KP_0 : KEY_SPACE).active && m_cooldown == 0)
		{
			Fire();
			m_cooldown = 0.1;
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

		m_fEnergy -= Time::frame * 0.5;
		if (m_fEnergy <= 0)
		{
			Die();
		}
	}

	void Die()
	{
		DestroySelf();
	}

	void Damage(int nDmg)
	{
		m_fHealth -= nDmg;
		if (m_fHealth <= 0) Die();
	}

	int m_nPlayer = 0;

};

void CProjectile::OnOverlap(const SOverlapEvent& event)
{

	CRat* pRat = dynamic_cast<CRat*>(event.pOther.get());
	if (pRat && pRat->m_nPlayer != m_nPlayer)
	{
		pRat->Damage(10);
		DestroySelf();
		return;
	}

	CDirt* pDirt = dynamic_cast<CDirt*>(event.pOther.get());
	if (pDirt)
	{
		pDirt->DestroySelf();
		DestroySelf();
		return;
	}
	
}

class CTestGame : public CGameLogic
{
	std::vector<CGuiImage*> aImages;
	std::shared_ptr<CGameObject> root;
	std::weak_ptr<CRat> m_rat1;
	std::weak_ptr<CRat> m_rat2;
	std::shared_ptr<CImageSprite> pBg;
	CCameraRenderLayer *pCamera1, *pCamera2;
	CGuiLayer *pGui;

	CGuiImage *pLeftBar1Bg, *pLeftBar2Bg, *pRightBar1Bg, *pRightBar2Bg;
	CGuiImage *pLeftHpBar, *pLeftEnergyBar, *pRightHpBar, *pRightEnergyBar;

	int nSizeX = 20, nSizeY = 15;

	virtual bool Load()
	{
		pCamera1 = new CCameraRenderLayer(IntRect(0, 0, 512, 512));
		pCamera2 = new CCameraRenderLayer(IntRect(512, 0, 512, 512));
		pCamera1->AddTag("camera");
		pCamera2->AddTag("camera");
		pCamera2->SetPosition(IntVec(0, 0));
		SDLManager::Instance.AddLayer(pCamera1);
		SDLManager::Instance.AddLayer(pCamera2);


		CTexture::LoadTexture("resources/bg.png");
		pBg = std::make_shared<CImageSprite>();
		pBg->SetTexture(CTexture::GetTexture("resources/bg.png"));
		pBg->SetSize(2048, 2048);
		pCamera1->AddSprite(pBg);
		pCamera2->AddSprite(pBg);

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
		rat2->SetPosition(Vec(300, 0));
		root->AttachGameObject(rat2, Vec(0, 0));
		m_rat2 = rat2;

		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 20; ++j)
			{
				auto dirt = std::make_shared<CDirt>();
				root->AttachGameObject(dirt, Vec(i*64, j*64));
			}
		}
		
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

	float fHp1, fHp2, fEnergy1, fEnergy2;

	void Resize()
	{
		IntVec vWinSize = SDLManager::GetSize();

		pCamera1->SetRect(IntRect(0, 0, vWinSize.x / 2, vWinSize.y));
		pCamera2->SetRect(IntRect(vWinSize.x / 2, 0, vWinSize.x / 2, vWinSize.y));
		pGui->SetRect(IntRect(0, 0, vWinSize.x, vWinSize.y));

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

	virtual void Update()
	{
		root->UpdateInternal(Vec(0, 0));

		Resize();

		auto rat1 = m_rat1.lock();
		if (rat1)
		{
			pCamera1->SetPosition(rat1->GetAbsolutePos() - (pCamera1->GetRect().GetSize() / 2));
		}

		auto rat2 = m_rat2.lock();
		if (rat2)
		{
			pCamera2->SetPosition(rat2->GetAbsolutePos() - (pCamera2->GetRect().GetSize() / 2));
		}
		
	}
};

int main(int argc, char* args[])
{
	SWindowParams Params;
	Params.nWidth = 800;
	Params.nHeight = 600;
	Params.sTitle = "Rats";
	Params.bResizeAble = true;
	Params.bMaximized = true;

	{
		CTestGame* pGame = new CTestGame();
		SDLManager::Start(Params, pGame);
	}
	//system("pause");
	return 0;
}

//http://lazyfoo.net/tutorials/SDL/index.php