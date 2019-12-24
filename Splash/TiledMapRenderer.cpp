#include "TiledMapRenderer.h"
#include "Input.h"
#include <cassert>
#include "SplashGame.h"

void CTiledMap::SetSize(IntVec vSize)
{
	m_vSize = vSize;
	m_aTiles.resize(vSize.x * vSize.y);
}

void CTiledMap::Render()
{
	if (!m_bVisible) return;
	
	//IntVec vRenderSize = { 4,4 }; //todo

	for (int y = 0; y < m_vSize.y; ++y)
	{
		for (int x = 0; x < m_vSize.x; ++x)
		{
			std::vector<SSpriteData>& aTiles = m_aTiles[y*m_vSize.x + x];
			for (unsigned int i = 0; i < aTiles.size(); ++i)
			{
				Vec vRenderPos = GetTileScreenCoord(Vec((float)x, (float)y) + aTiles[i].m_vPos - m_vCameraPos);
				vRenderPos.y += aTiles[i].m_fHeight * m_nTileHeight;
				aTiles[i].m_pSprite->Render(*this, vRenderPos);
			}
		}
	}

}

void CTiledMap::HandleEvents()
{
}

int CTiledMap::AddSprite(IntVec vTile, SSpriteData Sprite)
{
	auto& aTiles = GetSprites(vTile);
	aTiles.push_back(Sprite);
	return aTiles.size() - 1;
}

std::vector<SSpriteData>& CTiledMap::GetSprites(IntVec vPos)
{
	return m_aTiles[vPos.y * m_vSize.x + vPos.x];
}

void CTiledMap::UpdateMovement()
{
	//IntVec vTileOffset = GetTileScreenCoord({ 1,1 });

	if (Input::GetKey(KEY_LEFT).active)
	{
		m_vCameraPos.x += (float) Time::frame * 3;
	}
	if (Input::GetKey(KEY_RIGHT).active)
	{
		m_vCameraPos.x -= (float)Time::frame * 3;
	}
	if (Input::GetKey(KEY_UP).active)
	{
		m_vCameraPos.y -= (float)Time::frame * 3;
	}
	if (Input::GetKey(KEY_DOWN).active)
	{
		m_vCameraPos.y += (float)Time::frame * 3;
	}
}

void CTiledMap::SetCamera(Vec vPos, int nRotation)
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

Vec CTiledMap::GetTileScreenCoord(Vec vPos) const
{
	Vec vOut;
	switch (m_nRotation)
	{
	case 0:	return{ m_vTileSize.x * (+vPos.x - vPos.y), m_vTileSize.y * (+vPos.x + vPos.y) }; break;
	case 1:	return{ m_vTileSize.x * (-vPos.x - vPos.y), m_vTileSize.y * (+vPos.x - vPos.y) }; break;
	case 2:	return{ m_vTileSize.x * (-vPos.x + vPos.y), m_vTileSize.y * (-vPos.x - vPos.y) }; break;
	case 3:	return{ m_vTileSize.x * (+vPos.x + vPos.y), m_vTileSize.y * (-vPos.x + vPos.y) }; break;
	default:
		assert(0);
		return Vec();
	}
}
