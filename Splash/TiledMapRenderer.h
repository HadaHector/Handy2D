#pragma once
#include "RenderLayer.h"
#include <array>


struct SSpriteData
{
	SSpriteData() = default;
	SSpriteData(std::shared_ptr<CSprite> pSprite, int nHeight = 0, int nRotation = 0)
	{
		m_aSprites.push_back(pSprite);
		m_nHeight = nHeight;
		m_nRotation = nRotation;
	}
	SSpriteData(std::vector<std::shared_ptr<CSprite>> aSprites, int nHeight = 0, int nRotation = 0)
	{
		m_aSprites = aSprites;
		m_nHeight = nHeight;
		m_nRotation = nRotation;
	}
	std::vector<std::shared_ptr<CSprite>> m_aSprites;
	int m_nHeight;
	int m_nRotation;
};

struct STileSpriteData
{
	enum ETileSpriteSlot
	{
		Ground,
		Object,
		SmallObject0,
		SmallObject1,
		SmallObject2,
		SmallObject3,
		Side0,
		Side1,
		Side2,
		Side3,
		Cliff0,
		Cliff1,
		Cliff2,
		Cliff3,
		LAST
	};
	std::array<std::vector<SSpriteData>, (int)ETileSpriteSlot::LAST> m_aSprites;
};

class CTiledMapRenderer : public CRenderLayer
{
public:

	IntVec m_vSize;
	std::vector<STileSpriteData> m_aTiles;

	CTiledMapRenderer(const IntRect& rect) : CRenderLayer(rect) {};
	void SetSize(IntVec vSize);

	void SetCamera(IntVec vPos, int nRotation);
	IntVec GetTileScreenCoord(IntVec vPos) const;
	IntVec GetTileScreenCoordPixel(IntVec vPos) const;

	virtual void Render() override;
	virtual void HandleEvents() override;

	void AddSprite(IntVec vTile, STileSpriteData::ETileSpriteSlot eSlot, SSpriteData);
	STileSpriteData& GetTileData(IntVec vPos);

	void UpdateMovement();
private:

	IntVec m_vCameraPos;
	int m_nRotation = 0;

	IntVec m_vTileSize = { 32,16 };
	IntVec m_vTileSizePixel = { 2,1 };
	int nTilePixels = 16;
};


struct STileData
{
	int m_nHeigth;
	int m_aHeights[4];
};

class CTiledMap 
{
public:
	CTiledMap() : m_Renderer({ 0,0,512,512 }) {};

	CTiledMapRenderer m_Renderer;
	
	IntVec GetSize() const { return m_vSize; }
	void SetSize(IntVec vSize);
	
	STileData& GetTileData(IntVec vPos);
	int GetHeight(IntVec vPos);

	void InitTestMap();
	void UpdateTerrainSprites(IntVec vPos);

private:
	IntVec m_vSize;
	std::vector<STileData> m_aTiles;
};