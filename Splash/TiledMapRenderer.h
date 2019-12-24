#pragma once
#include "RenderLayer.h"


struct SSpriteData
{
	SSpriteData() = default;
	SSpriteData(std::shared_ptr<CSprite> pSprite, Vec vPos = Vec(0.0f, 0.0f), float fHeight = 0.0f)
	{
		m_pSprite = pSprite;
		m_vPos = vPos;
		m_fHeight = fHeight;
	}
	std::shared_ptr<CSprite> m_pSprite;
	Vec m_vPos;
	float m_fHeight;
};

class CTiledMap : public CRenderLayer
{
public:

	IntVec m_vSize;
	std::vector<std::vector<SSpriteData>> m_aTiles;

	CTiledMap(const IntRect& rect) : CRenderLayer(rect) {};
	void SetSize(IntVec vSize);

	void SetCamera(Vec vPos, int nRotation);
	IntVec GetTileScreenCoord(IntVec vPos) const;
	Vec GetTileScreenCoord(Vec vPos) const;

	virtual void Render() override;
	virtual void HandleEvents() override;

	int AddSprite(IntVec vTile, SSpriteData);
	std::vector<SSpriteData>& GetSprites(IntVec vPos);

	void UpdateMovement();
private:

	Vec m_vCameraPos;
	int m_nRotation = 0;

	IntVec m_vTileSize = { 32,16 };
	int m_nTileHeight = 16;
};