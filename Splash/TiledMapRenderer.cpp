#include "TiledMapRenderer.h"
#include "Input.h"
#include <cassert>
#include "SplashGame.h"

namespace
{
	std::vector<STileSpriteData::ETileSpriteSlot> g_aSlotRenderOrder[4] =
	{
		{ STileSpriteData::Cliff2, STileSpriteData::Cliff1, STileSpriteData::Ground, STileSpriteData::Side3, STileSpriteData::Side0, STileSpriteData::SmallObject0, STileSpriteData::SmallObject3, STileSpriteData::Object, STileSpriteData::SmallObject1, STileSpriteData::SmallObject2, STileSpriteData::Side2, STileSpriteData::Side1 },
		{ STileSpriteData::Cliff1, STileSpriteData::Cliff0, STileSpriteData::Ground, STileSpriteData::Side2, STileSpriteData::Side3, STileSpriteData::SmallObject1, STileSpriteData::SmallObject0, STileSpriteData::Object, STileSpriteData::SmallObject2, STileSpriteData::SmallObject3, STileSpriteData::Side1, STileSpriteData::Side0 },
		{ STileSpriteData::Cliff0, STileSpriteData::Cliff3, STileSpriteData::Ground, STileSpriteData::Side1, STileSpriteData::Side2, STileSpriteData::SmallObject2, STileSpriteData::SmallObject1, STileSpriteData::Object, STileSpriteData::SmallObject3, STileSpriteData::SmallObject0, STileSpriteData::Side0, STileSpriteData::Side3 },
		{ STileSpriteData::Cliff3, STileSpriteData::Cliff2, STileSpriteData::Ground, STileSpriteData::Side0, STileSpriteData::Side1, STileSpriteData::SmallObject3, STileSpriteData::SmallObject2, STileSpriteData::Object, STileSpriteData::SmallObject0, STileSpriteData::SmallObject1, STileSpriteData::Side3, STileSpriteData::Side2 }
	};
}


void CTiledMapRenderer::SetSize(IntVec vSize)
{
	m_vSize = vSize;
	m_aTiles.resize(vSize.x * vSize.y);
}

void CTiledMapRenderer::Render()
{
	if (!m_bVisible) return;
	
	//IntVec vRenderSize = { 4,4 }; //todo

	int nFromX, nFromY, nToX, nToY, nIncX, nIncY;
	switch (m_nRotation)
	{
	case 0:
		nFromX = 0;
		nToX = m_vSize.x;
		nIncX = 1;
		nFromY = 0;
		nToY = m_vSize.y;
		nIncY = 1;
		break;
	case 1:
		nFromX = 0;
		nToX = m_vSize.x;
		nIncX = 1;
		nFromY = m_vSize.y-1;
		nToY = -1;
		nIncY = -1;
		break;
	case 2:
		nFromX = m_vSize.x - 1;
		nToX = -1;
		nIncX = -1;
		nFromY = m_vSize.y-1;
		nToY = -1;
		nIncY = -1;
		break;
	case 3:
		nFromX = m_vSize.x - 1;
		nToX = -1;
		nIncX = -1;
		nFromY = 0;
		nToY = m_vSize.y;
		nIncY = 1;
		break;	
	}

	for (int y = nFromY; y != nToY; y += nIncY)
	{
		for (int x = nFromX; x != nToX; x += nIncX)
		{
			STileSpriteData& Tile = m_aTiles[y*m_vSize.x + x];

			for (unsigned int slot = 0; slot < g_aSlotRenderOrder[m_nRotation].size(); ++slot)
			{
				std::vector<SSpriteData>& aTiles = Tile.m_aSprites[g_aSlotRenderOrder[m_nRotation][slot]];
				for (unsigned int i = 0; i < aTiles.size(); ++i)
				{
					Vec vRenderPos = GetTileScreenCoordPixel(IntVec(x * nTilePixels, y * nTilePixels) - m_vCameraPos) + m_Rect.GetSize() / 2;
					vRenderPos.y -= aTiles[i].m_nHeight;
					aTiles[i].m_aSprites[(m_nRotation + aTiles[i].m_nRotation) % (aTiles[i].m_aSprites.size())]->Render(*this, vRenderPos);
				}
			}

			
		}
	}

}

void CTiledMapRenderer::HandleEvents()
{
}

void CTiledMapRenderer::AddSprite(IntVec vTile, STileSpriteData::ETileSpriteSlot eSlot, SSpriteData Sprite)
{
	auto& aTiles = GetTileData(vTile);
	aTiles.m_aSprites[eSlot].push_back(Sprite);
}

STileSpriteData& CTiledMapRenderer::GetTileData(IntVec vPos)
{
	return m_aTiles[vPos.y * m_vSize.x + vPos.x];
}

void CTiledMapRenderer::UpdateMovement()
{
	int nDir = -1;
	if (Input::GetKey(KEY_LEFT).active || Input::GetMousePos().x < 10)
	{
		nDir = 3;
	}
	if (Input::GetKey(KEY_RIGHT).active || Input::GetMousePos().x > GetRect().GetSize().x - 10)
	{
		nDir = 1;
	}
	if (Input::GetKey(KEY_UP).active || Input::GetMousePos().y < 10)
	{
		nDir = 2;
	}
	if (Input::GetKey(KEY_DOWN).active || Input::GetMousePos().y > GetRect().GetSize().y - 10)
	{
		nDir = 0;
	}

	if (nDir != -1)
	{
		int nNewDir = (nDir + m_nRotation) % 4;

		int nSignX, nSignY;
		switch (nNewDir)
		{
		case 0: nSignX = 1; nSignY = 1; break;
		case 1: nSignX = 1; nSignY = -1; break;
		case 2: nSignX = -1; nSignY = -1; break;
		case 3: nSignX = -1; nSignY = 1; break;
		}

		if (nDir == 0 || nDir == 2)
		{
			nSignX *= 2;
			nSignY *= 2;
		}

		m_vCameraPos.x += (int)(Time::frame * 100) * nSignX;
		m_vCameraPos.y += (int)(Time::frame * 100) * nSignY;
	}

	

	if (Input::GetKey(MOUSE_LEFT).pressed)
	{
		m_nRotation = (m_nRotation + 1) % 4;
	}
}

void CTiledMapRenderer::SetCamera(IntVec vPos, int nRotation)
{
	m_vCameraPos = vPos;
	m_nRotation = nRotation;
}

IntVec CTiledMapRenderer::GetTileScreenCoord(IntVec vPos) const
{
	IntVec vOut;
	switch (m_nRotation)
	{
	case 0:	return{ m_vTileSize.x * (+vPos.x -vPos.y), m_vTileSize.y * (+vPos.x +vPos.y) }; break;
	case 1:	return{ m_vTileSize.x * (-vPos.x -vPos.y), m_vTileSize.y * (+vPos.x -vPos.y) }; break;
	case 2:	return{ m_vTileSize.x * (-vPos.x +vPos.y), m_vTileSize.y * (-vPos.x -vPos.y) }; break;
	case 3:	return{ m_vTileSize.x * (+vPos.x +vPos.y), m_vTileSize.y * (-vPos.x +vPos.y) }; break;
	default:
		assert(0);
		return IntVec();
	}
}

IntVec CTiledMapRenderer::GetTileScreenCoordPixel(IntVec vPos) const
{
	Vec vOut;
	switch (m_nRotation)
	{
	case 0:	return{ m_vTileSizePixel.x * (+vPos.x - vPos.y), m_vTileSizePixel.y * (+vPos.x + vPos.y) }; break;
	case 1:	return{ m_vTileSizePixel.x * (-vPos.x - vPos.y), m_vTileSizePixel.y * (+vPos.x - vPos.y) }; break;
	case 2:	return{ m_vTileSizePixel.x * (-vPos.x + vPos.y), m_vTileSizePixel.y * (-vPos.x - vPos.y) }; break;
	case 3:	return{ m_vTileSizePixel.x * (+vPos.x + vPos.y), m_vTileSizePixel.y * (-vPos.x + vPos.y) }; break;
	default:
		assert(0);
		return Vec();
	}
}

void CTiledMap::SetSize(IntVec vSize)
{
	m_vSize = vSize;
	m_aTiles.resize(m_vSize.x*m_vSize.y);
	m_Renderer.SetSize(vSize);
}

STileData & CTiledMap::GetTileData(IntVec vPos)
{
	if (vPos.x < 0 || vPos.x >= m_vSize.x || vPos.y < 0 || vPos.y >= m_vSize.y) throw(0);
	return m_aTiles[vPos.y * m_vSize.x + vPos.x];
}


#include <random>
void CTiledMap::InitTestMap()
{
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> random_4(0, 3);

	IntVec vMapSize = { 10,10 };
	SetSize(vMapSize);

	for (int y = 0; y < vMapSize.y; ++y)
	{
		for (int x = 0; x < vMapSize.x; ++x)
		{
			int nHeight = std::max(0,random_4(e1) + 1);
			GetTileData({ x,y }).m_nHeigth = nHeight;

			std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
			pSprite->SetTexture(CTexture::GetTexture(x % 4 == 0 ? "tile2" : "tile"));
			pSprite->SetSize({ 64,32 });
			m_Renderer.AddSprite({ x,y }, STileSpriteData::Ground, { pSprite, nHeight * 16 });
			
			if (random_4(e1) == 0)
			{
				std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
				pSprite->SetTexture(CTexture::GetTexture("tree"));
				pSprite->SetSize({ 64,64 });
				m_Renderer.AddSprite({ x,y }, STileSpriteData::Object, { pSprite, 32 + nHeight * 16, random_4(e1) });
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
				m_Renderer.AddSprite({ x,y }, STileSpriteData::Object, { aSprites, 32 + nHeight * 16, random_4(e1) });
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
				m_Renderer.AddSprite({ x,y }, (STileSpriteData::ETileSpriteSlot) ((int)STileSpriteData::Side0 + nSide), { aSprites, 16 + nHeight * 16, nSide });
			}
			
		}
	}

	for (int y = 0; y < vMapSize.y; ++y)
	{
		for (int x = 0; x < vMapSize.x; ++x)
		{
			UpdateTerrainSprites({ x,y });
		}
	}
	

	
}


int CTiledMap::GetHeight(IntVec vPos)
{
	if (vPos.x < 0 || vPos.x >= m_vSize.x || vPos.y < 0 || vPos.y >= m_vSize.y) return 0;
	return m_aTiles[vPos.y * m_vSize.x + vPos.x].m_nHeigth;
}

void CTiledMap::UpdateTerrainSprites(IntVec vPos)
{
	STileData& Data = GetTileData(vPos);
	STileSpriteData& SpriteData = m_Renderer.GetTileData(vPos);

	IntVec vNeighbour;
	std::vector<std::string> sTex;
	for (int i = 0; i < 4; ++i)
	{
		SpriteData.m_aSprites[STileSpriteData::Cliff0 + i].clear();
		vNeighbour = vPos;
		switch (i)
		{
		case 0: 
			vNeighbour.y--;
			sTex = { "","wall_right","wall_left","" };
			break;
		case 1:
			vNeighbour.x++;
			sTex = { "wall_right","wall_left", "", "" };
			break;
		case 2:
			vNeighbour.y++;
			sTex = { "wall_left", "", "", "wall_right" };
			break;
		case 3:
			vNeighbour.x--;
			sTex = { "", "","wall_right","wall_left" };
			break;
		}

		int nHeight = GetHeight(vNeighbour);
		for (int j = nHeight; j < Data.m_nHeigth; ++j)
		{
			std::vector<std::shared_ptr<CSprite>> aSprites;
			for (int k = 0; k < 4; ++k)
			{
				std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
				pSprite->SetTexture(CTexture::GetTexture(sTex[k]));
				pSprite->SetSize({ 64,32 });
				aSprites.push_back(pSprite);
			}
			SpriteData.m_aSprites[STileSpriteData::Cliff0 + i].push_back({
				aSprites, j * 16, 0
			});
		}
	}
}
