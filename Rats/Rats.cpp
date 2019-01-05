#include "SDLManager.h"
#include "RenderLayer.h"
#include <array>
#include "Input.h"
#include <algorithm>
#include "GameObject.h"
#include "GuiLayer.h"
#include <random>
#include "CollisionManager.h"


class CProjectile : public CGameObject
{
	Vec m_vSpeed;
	int m_nPlayer;
public:
	CProjectile()
	{
		CTexture::LoadTexture("resources/projectile.png");
		std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
		pSprite->SetTexture(CTexture::GetTexture("resources/projectile.png"));
		pSprite->SetSize(8, 8);
		AttachSprite(pSprite, Vec(-4, -4), { "camera" });
		AddCollider(IntRect(-4, -4, 8, 8), true, true);
	}

	virtual void Update()
	{
		m_vPosition += m_vSpeed * Time::frame;
	}

	void SetSpeed(const Vec& spd)
	{
		m_vSpeed = spd;
	}

	void SetPlayer(int nPlayer)
	{
		m_nPlayer = nPlayer;
	}

	virtual void OnCollide(const SOverlapEvent& event) override;

};


class CRat : public CGameObject
{
private:
	int m_nStep = 0;
	double m_timer = 0;
	double m_cooldown = 0;
	std::shared_ptr<CAnimSprite> m_pRat;

	int m_nHealth = 100;
public:
	CRat()
	{
		m_vPosition = Vec(0, 0);
		CTexture::LoadTexture("resources/rat_left.png");
		CTexture::LoadTexture("resources/rat_right.png");
		m_pRat = std::make_shared<CAnimSprite>();
		m_pRat->SetTexture(CTexture::GetTexture("resources/rat_left.png"));
		m_pRat->SetRowsAndCols(4, 1);
		m_pRat->SetSize(170,64);
		AttachSprite(m_pRat, Vec(-85, -32), {"camera"});
		AddCollider(IntRect(-85, -32, 170, 64), true, true);
	}

	void Fire()
	{
		auto bullet = std::make_shared<CProjectile>();
		bullet->SetSpeed(Vec(1000, 0));
		bullet->SetPlayer(m_nPlayer);
		GetParent()->AttachGameObject(bullet, m_vPosition);
	}

	virtual void Update()
	{
		bool bMoving = false;
		if (Input::GetKey(m_nPlayer ? KEY_RIGHT : KEY_D).active)
		{
			m_vPosition.x += Time::frame * 200;
			m_pRat->SetTexture(CTexture::GetTexture("resources/rat_right.png"));
			m_pRat->SetSize(170, 64);
			bMoving = true;
		}
		if (Input::GetKey(m_nPlayer ? KEY_LEFT : KEY_A).active)
		{
			m_vPosition.x -= Time::frame * 200;
			m_pRat->SetTexture(CTexture::GetTexture("resources/rat_left.png"));
			m_pRat->SetSize(170, 64);
			bMoving = true;
		}
		if (Input::GetKey(m_nPlayer ? KEY_DOWN : KEY_S).active)
		{
			m_vPosition.y += Time::frame * 200;
			bMoving = true;
		}
		if (Input::GetKey(m_nPlayer ? KEY_UP : KEY_W).active)
		{
			m_vPosition.y -= Time::frame * 200;
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
	}

	void Die()
	{
		DestroySelf();
	}

	void Damage(int nDmg)
	{
		m_nHealth -= nDmg;
		if (m_nHealth <= 0) Die();
	}

	int m_nPlayer = 0;

};

void CProjectile::OnCollide(const SOverlapEvent& event)
{
	CRat* pRat = dynamic_cast<CRat*>(event.pOther);
	if (pRat && pRat->m_nPlayer != m_nPlayer)
	{
		pRat->Damage(10);
		DestroySelf();
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

		return true;
	}

	virtual void Update()
	{
		root->UpdateInternal(Vec(0, 0));

		IntVec vWinSize = SDLManager::GetSize();

		pCamera1->SetRect(IntRect(0, 0, vWinSize.x / 2, vWinSize.y));
		pCamera2->SetRect(IntRect(vWinSize.x / 2, 0, vWinSize.x / 2, vWinSize.y));

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