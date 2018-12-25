#include "SDLManager.h"
#include "RenderLayer.h"
#include <array>
#include "Input.h"
#include <algorithm>
#include "GameObject.h"
#include "GuiLayer.h"
#include <random>

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
	std::vector<ETileState> tiles;
	std::vector<CGuiImage*> aImages;

	int nSizeX = 20, nSizeY = 15;

	int CountMines(int x, int y)
	{
		int nCount = 0;
		for (int x2 = -1; x2<2; ++x2)
			for (int y2 = -1; y2 < 2; ++y2)
			{
				if (x + x2 >= 0 && x + x2 < nSizeX && y + y2 >= 0 && y + y2 < nSizeY)
				{
					int idx2 = (y + y2) * nSizeX + x + x2;
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
		for (int y = 0; y<nSizeY; ++y)
			for (int x = 0; x<nSizeX; ++x)
			{
				int idx = y * nSizeX + x;
				std::string sImage;
				switch (tiles[idx])
				{
				case ETS_Clear_Unused: sImage = "resources\\default.png"; break;
				case ETS_Clear_Flagged: sImage = "resources\\flagged.png"; break;
				case ETS_Mine_Unused: sImage = "resources\\default.png"; break;
				case ETS_Mine_Flagged: sImage = "resources\\flagged.png"; break;
				case ETS_Mine_Used: sImage = "resources\\exploded.png"; break;
				case ETS_Mine_Shown: sImage = "resources\\mine.png"; break;
				case ETS_Clear_Used:
					sImage = "resources\\";
					sImage += std::to_string(CountMines(x, y));
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
			for (int y = 0; y < nSizeY; ++y)
				for (int x = 0; x < nSizeX; ++x)
				{
					if (tiles[y * nSizeX + x] == ETS_Clear_Used && CountMines(x, y) == 0)
					{
						for (int x2 = -1; x2 < 2; ++x2)
							for (int y2 = -1; y2 < 2; ++y2)
							{
								if (x + x2 >= 0 && x + x2 < nSizeX && y + y2 >= 0 && y + y2 < nSizeY)
								{
									int idx2 = (y + y2) * nSizeX + x + x2;
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
		for (int y = 0; y<nSizeY; ++y)
			for (int x = 0; x<nSizeX; ++x)
			{
				int idx = y * nSizeX + x;
				if (tiles[idx] == ETS_Mine_Unused) tiles[idx] = ETS_Mine_Shown;
			}
	}

	virtual bool Load()
	{

		tiles.reserve(nSizeX*nSizeY);
		for (int i = 0; i<nSizeX*nSizeY; ++i)
		{
			tiles.push_back(ETS_Clear_Unused);
		}

		for (int i = 0; i < 15; ++i)
		{
			tiles[rand() % (nSizeX*nSizeY)] = ETS_Mine_Unused;
		}

		CGuiLayer* pGui = new CGuiLayer(800, 600);
		SDLManager::Instance.AddLayer(pGui);

		auto fnTileClick = [=](SClickEvent& Event) {

			int x = atoi(Event.m_pInitiator->Properties()["x"].c_str());
			int y = atoi(Event.m_pInitiator->Properties()["y"].c_str());

			ETileState& state = tiles[y * nSizeX + x];
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

		CTexture::LoadTexture("resources\\default.png");
		CTexture::LoadTexture("resources\\mine.png");
		CTexture::LoadTexture("resources\\flagged.png");
		CTexture::LoadTexture("resources\\exploded.png");
		CTexture::LoadTexture("resources\\0.png");
		CTexture::LoadTexture("resources\\1.png");
		CTexture::LoadTexture("resources\\2.png");
		CTexture::LoadTexture("resources\\3.png");
		CTexture::LoadTexture("resources\\4.png");
		CTexture::LoadTexture("resources\\5.png");
		CTexture::LoadTexture("resources\\6.png");
		CTexture::LoadTexture("resources\\7.png");
		CTexture::LoadTexture("resources\\8.png");

		for (int i = 0; i<nSizeY; ++i)
			for (int j = 0; j<nSizeX; ++j)
			{
				CGuiImage* pImage = new CGuiImage();
				pImage->SetPosition(IntVec(100 + 25 * j, 100 + 25 * i));
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

	}
};

int main(int argc, char* args[])
{
	SWindowParams Params;
	Params.nWidth = 800;
	Params.nHeight = 600;
	Params.sTitle = "Minesweeper";

	{
		CTestGame* pGame = new CTestGame();
		SDLManager::Start(Params, pGame);
	}
	//system("pause");
	return 0;
}

//http://lazyfoo.net/tutorials/SDL/index.php