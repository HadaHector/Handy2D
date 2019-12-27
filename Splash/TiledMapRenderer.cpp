#include "TiledMapRenderer.h"
#include "Input.h"
#include <cassert>
#include "SplashGame.h"
#include "Assets.h"

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

	m_aSpritesUnderCursor.clear();

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
					std::shared_ptr<CImageSprite> pSprite = aTiles[i].m_aSprites[(m_nRotation + aTiles[i].m_nRotation) % (aTiles[i].m_aSprites.size())];
					pSprite->Render(*this, vRenderPos);

					IntRect Rect;
					Rect.SetPos(vRenderPos);
					Rect.SetSize(pSprite->GetSize());
					IntVec vMousePos = Input::GetMousePos();
					if (Rect.IsInside(vMousePos))
					{
						SSpriteBinding Binding;
						Binding.m_vPos = { x,y };
						Binding.m_pSprite = pSprite;
						Binding.m_vClickCoord = vMousePos - vRenderPos;
						m_aSpritesUnderCursor.push_back(Binding);
					}
					
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
#include <Windows.h>

void CTiledMapRenderer::Update()
{
	//if (Input::GetKey(MOUSE_RIGHT).pressed)
	{
		if (m_vSelectorPos != IntVec(-1, -1))
		{
			if (GetTileData(m_vSelectorPos).m_aSprites[STileSpriteData::Ground].back().m_sData == "selector")
			{
				GetTileData(m_vSelectorPos).m_aSprites[STileSpriteData::Ground].pop_back();
			}			
		}

		m_vSelectorPos = { -1,-1 };

		if (m_aSpritesUnderCursor.size() > 0)
		{
			for (int i = m_aSpritesUnderCursor.size() - 1; i >= 0; --i)
			{
				bool bOk = false;

				std::shared_ptr<CImageSprite> pSprite = m_aSpritesUnderCursor[i].m_pSprite.lock();
				if (pSprite && pSprite->GetTexture())
				{
					std::shared_ptr<CTexture> pTex = pSprite->GetTexture().m_tex.lock();
					if (pTex)
					{
						Color c = pTex->GetPixel(m_aSpritesUnderCursor[i].m_vClickCoord);
						if (c.a > 0)
						{
							bOk = true;
						}
					}
					
				}
				if (bOk)
				{
					m_vSelectorPos = m_aSpritesUnderCursor[i].m_vPos;

					std::shared_ptr<CImageSprite> pSelectorSprite = std::make_shared<CImageSprite>();
					pSelectorSprite->SetTexture(CTexture::GetTexture("selector"));
					pSelectorSprite->SetSize({ 64,64 });

					int nHeight = GetTileData(m_vSelectorPos).m_aSprites[STileSpriteData::Ground][0].m_nHeight;
					SSpriteData Data = { pSelectorSprite, nHeight };
					Data.m_sData = "selector";
					GetTileData(m_vSelectorPos).m_aSprites[STileSpriteData::Ground].push_back(Data);
					break;
				}
			}
			
		}
		else
		{
			m_vSelectorPos = { -1,-1 };
		}
	}

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

	

	if (Input::GetKey(MOUSE_RIGHT).pressed)
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

	IntVec vMapSize = { 20,20 };
	SetSize(vMapSize);

	for (int y = 0; y < vMapSize.y; ++y)
	{
		for (int x = 0; x < vMapSize.x; ++x)
		{
			STileData& Tile = GetTileData({ x,y });

			int nHeight = 3;
			if (x <= 4 && x>=2 && y >= 2 && y<= 4)
			{
				Tile.m_nHeigth = 2;
				Tile.m_aHeights[0] = nHeight;
				Tile.m_aHeights[1] = nHeight;
				Tile.m_aHeights[2] = nHeight;
				Tile.m_aHeights[3] = nHeight;
			}
			else
			{
				Tile.m_nHeigth = nHeight;
				Tile.m_aHeights[0] = nHeight;
				Tile.m_aHeights[1] = nHeight;
				Tile.m_aHeights[2] = nHeight;
				Tile.m_aHeights[3] = nHeight;
			}
			

			if (x == 2)
			{
				if (y == 2)
				{
					Tile.m_aHeights[2] = 2;
					Tile.m_nHeigth = nHeight;
				}
				if (y == 3)
				{
					Tile.m_aHeights[1] = 2;
					Tile.m_aHeights[2] = 2;
				}
				if (y == 4)
				{
					Tile.m_aHeights[1] = 2;
					Tile.m_nHeigth = nHeight;
				}
			}
			if (x == 3)
			{
				if (y == 2)
				{
					Tile.m_aHeights[2] = 2;
					Tile.m_aHeights[3] = 2;
				}
				if (y == 3)
				{
					Tile.m_aHeights[0] = 2;
					Tile.m_aHeights[1] = 2;
					Tile.m_aHeights[2] = 2;
					Tile.m_aHeights[3] = 2;
				}
				if (y == 4)
				{
					Tile.m_aHeights[0] = 2;
					Tile.m_aHeights[1] = 2;
				}
			}
			if (x == 4)
			{
				if (y == 2)
				{
					Tile.m_aHeights[3] = 2;
					Tile.m_nHeigth = nHeight;
				}
				if (y == 3)
				{
					Tile.m_aHeights[0] = 2;
					Tile.m_aHeights[3] = 2;
				}
				if (y == 4)
				{
					Tile.m_aHeights[0] = 2;
					Tile.m_nHeigth = nHeight;
				}
			}

			
			if (random_4(e1) == 0)
			{
				std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
				pSprite->SetTexture(CTexture::GetTexture("resources/tree.png"));
				pSprite->SetSize({ 64,64 });
				m_Renderer.AddSprite({ x,y }, STileSpriteData::Object, { pSprite, 32 + nHeight * 16, random_4(e1) });
			}
			/*
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
			*/
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

void CTiledMap::UpdateTerrainSprites(IntRect vArea)
{
	for (int y = 0; y < vArea.GetSize().y; ++y)
	{
		for (int x = 0; x < vArea.GetSize().x; ++x)
		{
			IntVec vPos = vArea.GetUpperLeft();
			vPos.x += x;
			vPos.y += y;
			if (vPos.x < 0 || vPos.x >= m_vSize.x || vPos.y < 0 || vPos.y >= m_vSize.y) continue;
			UpdateTerrainSprites(vPos);
		}
	}
}

void CTiledMap::UpdateTerrainSprites(IntVec vPos)
{
	STileData& Data = GetTileData(vPos);
	STileSpriteData& SpriteData = m_Renderer.GetTileData(vPos);

	//maga a terep

	unsigned char flags = 0x00000000;
	unsigned char nLow = 0, nHigh = 0;
	//0-3 bit: is lower, 4-7 bit is higher
	for (int i = 0; i < 4; ++i)
	{
		if (Data.m_aHeights[i] > Data.m_nHeigth)
		{
			flags |= (char)1 << (7 - (i + 4)); 
			nHigh++;
		}
		else if (Data.m_aHeights[i] < Data.m_nHeigth)
		{
			flags |= (char)1 << (7 - i);
			nLow++;
		}
	}


	static std::vector<STextureRef> aFlatTex = { CTexture::GetTexture("tiles/0001") };
	static std::vector<STextureRef> aOneDownTex = { 
		CTexture::GetTexture("tiles/0003"),
		CTexture::GetTexture("tiles/0004"),
		CTexture::GetTexture("tiles/0005"),
		CTexture::GetTexture("tiles/0002") };
	static std::vector<STextureRef> aOneDownOneUpTex = {
		CTexture::GetTexture("tiles/0013"),
		CTexture::GetTexture("tiles/0010"),
		CTexture::GetTexture("tiles/0011"),
		CTexture::GetTexture("tiles/0012") };
	static std::vector<STextureRef> aOneUpTex = {
		CTexture::GetTexture("tiles/0007"),
		CTexture::GetTexture("tiles/0008"),
		CTexture::GetTexture("tiles/0009"),
		CTexture::GetTexture("tiles/0006") };
	static std::vector<STextureRef> aSlideTex = {
		CTexture::GetTexture("tiles/0015"),
		CTexture::GetTexture("tiles/0016"),
		CTexture::GetTexture("tiles/0017"),
		CTexture::GetTexture("tiles/0014") };
	static std::vector<STextureRef> aValleyTex = {
		CTexture::GetTexture("tiles/0018"),
		CTexture::GetTexture("tiles/0019") };

	std::vector<STextureRef>* pTextures = nullptr;
	int nRot = 0;
	
	switch (flags)
	{
	case 0b00000000: pTextures = &aFlatTex; nRot = 0; break;
	case 0b00001000: pTextures = &aOneUpTex; nRot = 0; break;
	case 0b00000100: pTextures = &aOneUpTex; nRot = 1; break;
	case 0b00000010: pTextures = &aOneUpTex; nRot = 2; break;
	case 0b00000001: pTextures = &aOneUpTex; nRot = 3; break;
	case 0b00001100: pTextures = &aSlideTex; nRot = 0; break;
	case 0b00000110: pTextures = &aSlideTex; nRot = 1; break;
	case 0b00000011: pTextures = &aSlideTex; nRot = 2; break;
	case 0b00001001: pTextures = &aSlideTex; nRot = 3; break;
	case 0b00001010: pTextures = &aValleyTex; nRot = 0; break;
	case 0b00000101: pTextures = &aValleyTex; nRot = 1; break;
	case 0b10000000: pTextures = &aOneDownTex; nRot = 0; break;
	case 0b01000000: pTextures = &aOneDownTex; nRot = 1; break;
	case 0b00100000: pTextures = &aOneDownTex; nRot = 2; break;
	case 0b00010000: pTextures = &aOneDownTex; nRot = 3; break;
	case 0b10000010: pTextures = &aOneDownOneUpTex; nRot = 0; break;
	case 0b01000001: pTextures = &aOneDownOneUpTex; nRot = 1; break;
	case 0b00101000: pTextures = &aOneDownOneUpTex; nRot = 2; break;
	case 0b00010100: pTextures = &aOneDownOneUpTex; nRot = 3; break;
	}

	SpriteData.m_aSprites[STileSpriteData::Ground].clear();
	if (pTextures)
	{
		std::vector<std::shared_ptr<CImageSprite>> aSprites;
		for (unsigned int i = 0; i < pTextures->size(); ++i)
		{
			std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
			pSprite->SetTexture((*pTextures)[i]);
			pSprite->SetSize({ 64,64 });
			aSprites.push_back(pSprite);
		}
		SpriteData.m_aSprites[STileSpriteData::Ground].push_back({ aSprites, Data.m_nHeigth * 16 + 16, nRot });

	}
	


	//a cliff

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
			std::vector<std::shared_ptr<CImageSprite>> aSprites;
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

void CTiledMap::Update()
{
	m_Renderer.Update();

	if (Input::GetKey(MOUSE_LEFT).pressed)
	{
		if (m_Renderer.GetSelectorPos() != IntVec(-1, -1))
		{
			if (m_eMode == EInteraction_ObjectBuild)
			{
				CreateObject(m_pAssetToBuild, m_Renderer.GetSelectorPos());
			}
			if (m_eMode == EInteraction_TerrainUp)
			{
				GetTileData(m_Renderer.GetSelectorPos()).m_nHeigth++;
				for (int i = 0; i < 4; ++i)
				{
					GetTileData(m_Renderer.GetSelectorPos()).m_aHeights[i]++;
				}
				IntRect Rect;
				Rect.SetPos(m_Renderer.GetSelectorPos());
				Rect.SetSize({ 1,1 });
				Rect.Expand(1);
				UpdateTerrainSprites(Rect);
			}
			if (m_eMode == EInteraction_TerrainDown)
			{
				GetTileData(m_Renderer.GetSelectorPos()).m_nHeigth--;
				for (int i = 0; i < 4; ++i)
				{
					GetTileData(m_Renderer.GetSelectorPos()).m_aHeights[i]--;
				}
				IntRect Rect;
				Rect.SetPos(m_Renderer.GetSelectorPos());
				Rect.SetSize({ 1,1 });
				Rect.Expand(1);
				UpdateTerrainSprites(Rect);
			}
		}
	}
}

void CTiledMap::CreateObject(const CAsset* pAsset, IntVec vPos, int nRotation, int nHeight)
{
	if (!pAsset) return;

	if (nHeight == -1)
	{
		nHeight = GetHeight(vPos) * 16;
	}

	if (pAsset->GetSpriteSlot() == STileSpriteData::Object)
	{
		SSpriteData Data = pAsset->GetSpriteData();
		Data.m_nHeight += nHeight;
		Data.m_nRotation = nRotation;
		m_Renderer.AddSprite(vPos, STileSpriteData::Object, Data);
	}
	//todo else others
}

void CTiledMap::SetObjectToBuild(const CAsset* pAsset)
{
	m_pAssetToBuild = pAsset;
}

void CTiledMap::SetInteractionMode(EInteractionMode eMode)
{
	m_eMode = eMode;
}
