#include "TiledMapRenderer.h"
#include "Input.h"
#include <cassert>
#include "SplashGame.h"

namespace
{
	std::vector<STileData::ETileSpriteSlot> g_aSlotRenderOrder[4] =
	{
		{ STileData::Ground, STileData::Side3, STileData::Side0, STileData::SmallObject0, STileData::SmallObject3, STileData::Object, STileData::SmallObject1, STileData::SmallObject2, STileData::Side2, STileData::Side1 },
		{ STileData::Ground, STileData::Side2, STileData::Side3, STileData::SmallObject1, STileData::SmallObject0, STileData::Object, STileData::SmallObject2, STileData::SmallObject3, STileData::Side1, STileData::Side0 },
		{ STileData::Ground, STileData::Side1, STileData::Side2, STileData::SmallObject2, STileData::SmallObject1, STileData::Object, STileData::SmallObject3, STileData::SmallObject0, STileData::Side0, STileData::Side3 },
		{ STileData::Ground, STileData::Side0, STileData::Side1, STileData::SmallObject3, STileData::SmallObject2, STileData::Object, STileData::SmallObject0, STileData::SmallObject1, STileData::Side3, STileData::Side2 }
	};
}


void CTiledMap::SetSize(IntVec vSize)
{
	m_vSize = vSize;
	m_aTiles.resize(vSize.x * vSize.y);
}

void CTiledMap::Render()
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
			STileData& Tile = m_aTiles[y*m_vSize.x + x];

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

void CTiledMap::HandleEvents()
{
}

void CTiledMap::AddSprite(IntVec vTile, STileData::ETileSpriteSlot eSlot, SSpriteData Sprite)
{
	auto& aTiles = GetTileData(vTile);
	aTiles.m_aSprites[eSlot].push_back(Sprite);
}

STileData& CTiledMap::GetTileData(IntVec vPos)
{
	return m_aTiles[vPos.y * m_vSize.x + vPos.x];
}

void CTiledMap::UpdateMovement()
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

void CTiledMap::SetCamera(IntVec vPos, int nRotation)
{
	m_vCameraPos = vPos;
	m_nRotation = nRotation;
}

IntVec CTiledMap::GetTileScreenCoord(IntVec vPos) const
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

IntVec CTiledMap::GetTileScreenCoordPixel(IntVec vPos) const
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
