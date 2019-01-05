#include "SDLManager.h"
#include "RenderLayer.h"
#include <array>
#include "Input.h"
#include <algorithm>
#include "GameObject.h"
#include "GuiLayer.h"
#include <random>

class EnemyShip : public CGameObject
{
	std::shared_ptr<CAnimSprite> pShip;
	bool bMoveRight = true;
	float fVelocity = 0.0f;
public:
	EnemyShip()
	{
		CTexture::LoadTexture("ship1.png");

		pShip = std::make_shared<CAnimSprite>();
		pShip->SetTexture(CTexture::GetTexture("ship1.png"));
		pShip->SetRowsAndCols(9, 1);
		pShip->SetSize(128, 64);
		AttachSprite(pShip, Vec(-64, -32), {});
	}

	virtual void Update()
	{
		if (bMoveRight && m_vPosition.x > 800)
		{
			bMoveRight = false;
		}
		else
		if (!bMoveRight && m_vPosition.x < 250)
		{
			bMoveRight = true;
		}

		fVelocity = fVelocity + Time::frame * (bMoveRight ? 300 : -300);
		fVelocity = fmin(200, fmax(-200, fVelocity));

		pShip->SetFrame((int)roundf(4.0f + fVelocity / 50.0f));
		m_vPosition.x += fVelocity * Time::frame;
	}
};

class CRocket : public CGameObject
{
	std::shared_ptr<CImageSprite> pShip;
	float m_fDelta = 0.0f;

	bool bFired = false;
	float fDet = 0.0f;
public:
	CRocket(float fDelta)
	{
		CTexture::LoadTexture("ship.png");
		
		pShip = std::make_shared<CImageSprite>();
		pShip->SetTexture(CTexture::GetTexture("ship.png"));
		pShip->SetSize(20, 20);
		AttachSprite(pShip, Vec(-10, -10), {});

		m_fDelta = fDelta;
		
	}

	virtual void Update()
	{
		if (!bFired)
		{
			m_vPosition = Vec(sin(Time::full + m_fDelta) * 100, cos(Time::full + m_fDelta) * 100);
		}
		else
		{
			m_vPosition.y += Time::frame * -300;
			fDet += Time::frame;

			if (fDet > 1.0f)
			{
				DestroySelf();
			}
		}
	}

	void Fire()
	{
		bFired = true;
		DetachParent();
	}

};

class CShip : public CGameObject
{
	std::vector<std::shared_ptr<CRocket>> m_aRockets;
public: 
	CShip()
	{
		m_vPosition = Vec(500, 500);
		CTexture::LoadTexture("ship.png");
		{
			std::shared_ptr<CImageSprite> pShip = std::make_shared<CImageSprite>();
			pShip->SetTexture(CTexture::GetTexture("ship.png"));
			pShip->SetSize(58.0f, 77.0f);
			AttachSprite(pShip, Vec(-29, -35), {});
		}
		for (float i = 0.0f; i < 6.28f; ++i)
		{
			std::shared_ptr<CRocket> pTest = std::make_shared<CRocket>(i);
			AttachGameObject(pTest, Vec(0, 0));
			m_aRockets.push_back(pTest);
		}
	}

	virtual void Update()
	{
		if (Input::GetKey(KEY_RIGHT).active)
		{
			m_vPosition.x += Time::frame * 200;
		}
		if (Input::GetKey(KEY_LEFT).active)
		{
			m_vPosition.x -= Time::frame * 200;
		}

		
		if (Input::GetKey(KEY_SPACE).pressed)
		{
			if (m_aRockets.size() > 0)
			{
				m_aRockets[m_aRockets.size() - 1]->Fire();
				m_aRockets.pop_back();
			}
		}


	}

};

enum ETileState
{
	ETS_Clear_Unused,
	ETS_Mine_Unused,
	ETS_Clear_Flagged,
	ETS_Mine_Flagged,
	ETS_Clear_Used,
	ETS_Mine_Used,
	ETS_Mine_Shown
};

class CTestGame : public CGameLogic
{
	std::shared_ptr<CImageSprite> pShip;
	//std::array<std::shared_ptr<CAnimSprite>,100> aAnimSprites;
	std::shared_ptr<CTextSprite> pTextSprite;
	std::vector<std::shared_ptr<CSprite>> aLasers;
	double x = 480;

	std::weak_ptr<CTexture> lasertex;
	CSpriteRenderLayer* pLayer = nullptr;

	std::shared_ptr<CGameObject> root;
	std::shared_ptr<CGameObject> ship;
	std::vector<std::shared_ptr<CGameObject>> enemies;

	std::vector<ETileState> tiles;
	std::vector<CGuiImage*> aImages;

	int CountMines(int x, int y)
	{
		int nCount = 0;
		for (int x2 = -1; x2<2; ++x2)
			for (int y2 = -1; y2 < 2; ++y2)
			{
				if (x + x2 >= 0 && x + x2 < 10 && y + y2 >= 0 && y + y2 < 10)
				{
					int idx2 = (y + y2) * 10 + x + x2;
					if (tiles[idx2] == ETS_Mine_Unused || tiles[idx2] == ETS_Mine_Flagged || tiles[idx2] == ETS_Mine_Used)
					{
						nCount++;
					}
				}
			}
		return nCount;
	}

	void UpdateTiles()
	{
		for (int y = 0; y<10; ++y)
		for (int x = 0; x<10; ++x)
		{
			int idx = y * 10 + x;
			std::string sImage;
			switch (tiles[idx])
			{
			case ETS_Clear_Unused: sImage = "minesweeper\\default.png"; break;
			case ETS_Clear_Flagged: sImage = "minesweeper\\flagged.png"; break;
			case ETS_Mine_Unused: sImage = "minesweeper\\default.png"; break;
			case ETS_Mine_Flagged: sImage = "minesweeper\\flagged.png"; break;
			case ETS_Mine_Used: sImage = "minesweeper\\exploded.png"; break;
			case ETS_Mine_Shown: sImage = "minesweeper\\mine.png"; break;
			case ETS_Clear_Used:
				sImage = "minesweeper\\";
				sImage += std::to_string(CountMines(x,y));
				sImage += ".png";
				break;
			}

			aImages[idx]->SetImage(sImage);
		}
	}

	void CheckClearUsed()
	{
		bool nChanged;
		do
		{
			nChanged = false;
			for (int y = 0; y < 10; ++y)
			for (int x = 0; x < 10; ++x)
			{
				if (tiles[y * 10 + x] == ETS_Clear_Used && CountMines(x, y) == 0)
				{
					for (int x2 = -1; x2 < 2; ++x2)
					for (int y2 = -1; y2 < 2; ++y2)
					{
						if (x + x2 >= 0 && x + x2 < 10 && y + y2 >= 0 && y + y2 < 10)
						{
							int idx2 = (y + y2) * 10 + x + x2;
							if (tiles[idx2] != ETS_Clear_Used)
							{
								nChanged = true;
								tiles[idx2] = ETS_Clear_Used;
							}
						}
					}
				}
			}
		} while (nChanged);
	}

	void GameOver()
	{
		for (int y = 0; y<10; ++y)
		for (int x = 0; x<10; ++x)
		{
			int idx = y * 10 + x;
			if (tiles[idx] == ETS_Mine_Unused) tiles[idx] = ETS_Mine_Shown;
		}
	}

	virtual bool Load()
	{

		root = std::make_shared<CGameObject>();

		Vec Size = SDLManager::Instance.GetSize();
		pLayer = new CSpriteRenderLayer(IntRect(0,0,Size.x, Size.y));
		SDLManager::Instance.AddLayer(pLayer);

		ship = std::make_shared<CShip>();
		ship->AddToLayer(pLayer);	
		root->AttachGameObject(ship, Vec(0, 0));

		for (int i = 0; i < 6; ++i)
		{
			std::shared_ptr<CGameObject> enemy = std::make_shared<EnemyShip>();
			enemy->AddToLayer(pLayer);
			enemy->SetPosition(Vec(200 + 100* i, 50 + 64 * i));
			root->AttachGameObject(enemy, Vec(0, 0));
			enemies.push_back(enemy);
		}

		tiles.reserve(100);
		for(int i=0; i<100; ++i)
		{
			tiles.push_back(ETS_Clear_Unused);
		}

		for (int i = 0; i < 15; ++i)
		{
			tiles[rand() % 100] = ETS_Mine_Unused;
		}
		
		pTextSprite = std::make_shared<CTextSprite>();
		pTextSprite->SetPos(0.0f,0.0f);
		pTextSprite->SetSize(500.0f, 30.0f);
		pLayer->AddSprite(pTextSprite);
		

		CGuiLayer* pGui = new CGuiLayer(IntRect(0,0,800, 600));
		SDLManager::Instance.AddLayer(pGui);

		auto fnTileClick = [=](SClickEvent& Event) {

			int x = atoi(Event.m_pInitiator->Properties()["x"].c_str());
			int y = atoi(Event.m_pInitiator->Properties()["y"].c_str());

			ETileState& state = tiles[y * 10 + x];
			switch (state)
			{
			case ETS_Clear_Unused:
				if (Event.m_nButton == MOUSE_LEFT)
				{
					state = ETS_Clear_Used;
					CheckClearUsed();
				}
				else if (Event.m_nButton == MOUSE_RIGHT)
				{
					state = ETS_Clear_Flagged;
				}
				break;
			case ETS_Mine_Unused:
				if (Event.m_nButton == MOUSE_LEFT)
				{
					state = ETS_Mine_Used;
					GameOver();
				}
				else if (Event.m_nButton == MOUSE_RIGHT)
				{
					state = ETS_Mine_Flagged;
				}
				break;
			case ETS_Clear_Flagged:
				if (Event.m_nButton == MOUSE_RIGHT)
				{
					state = ETS_Clear_Unused;
				}
				break;
			case ETS_Mine_Flagged:
				if (Event.m_nButton == MOUSE_RIGHT)
				{
					state = ETS_Mine_Unused;
				}
				break;
			case ETS_Clear_Used:
				break;
			case ETS_Mine_Used:
				break;
			default:
				break;
			}

			UpdateTiles();
		};

		CTexture::LoadTexture("minesweeper\\default.png");
		CTexture::LoadTexture("minesweeper\\mine.png");
		CTexture::LoadTexture("minesweeper\\flagged.png");
		CTexture::LoadTexture("minesweeper\\exploded.png");
		CTexture::LoadTexture("minesweeper\\0.png");
		CTexture::LoadTexture("minesweeper\\1.png");
		CTexture::LoadTexture("minesweeper\\2.png");
		CTexture::LoadTexture("minesweeper\\3.png");
		CTexture::LoadTexture("minesweeper\\4.png");
		CTexture::LoadTexture("minesweeper\\5.png");
		CTexture::LoadTexture("minesweeper\\6.png");
		CTexture::LoadTexture("minesweeper\\7.png");
		CTexture::LoadTexture("minesweeper\\8.png");

		for (int i = 0; i<10; ++i)
			for (int j = 0; j<10; ++j)
		{
			CGuiImage* pImage = new CGuiImage();
			pImage->SetPosition(IntVec(100+25*j,100+25*i));
			aImages.push_back(pImage);
			pGui->GetRootElement()->AddChild(pImage);
			pImage->AddClickEventListener(fnTileClick);
			pImage->Properties().insert({ "x", std::to_string(j) });
			pImage->Properties().insert({ "y", std::to_string(i) });
		}

		UpdateTiles();
		
		return true;
	}

	virtual void Update()
	{
		root->UpdateInternal(Vec(0,0));

		
		std::string sTime = std::string("sprites: ");
		sTime += std::to_string(SDLManager::drawnum);
		sTime += "/";
		sTime += std::to_string( SDLManager::spritenum );
		pTextSprite->SetText(sTime);
		
	}
};

int main(int argc, char* args[])
{
	SWindowParams Params;
	Params.nWidth = 800;
	Params.nHeight = 600;
	Params.sTitle = "jeboi";

	{
		CTestGame* pGame = new CTestGame();
		SDLManager::Start(Params, pGame);
	}
	//system("pause");
	return 0;
}

//http://lazyfoo.net/tutorials/SDL/index.php