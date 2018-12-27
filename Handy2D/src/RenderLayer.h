#pragma once
#include "Sprite.h"
#include <vector>
#include <memory>

class CSprite;

class CRenderLayer
{
protected:
	bool m_bVisible = true;
	IntRect m_Size;
public:
	CRenderLayer(int x, int y) { m_Size = { 0,0,x,y }; }
	virtual ~CRenderLayer() {};
	virtual void Render() = 0;
	virtual void HandleEvents() = 0;
	void SetVisible(bool bVisible) { m_bVisible = bVisible; }
	bool IsVisible() const { return m_bVisible; }
	IntRect GetRect() const { return m_Size; }
};

class CSpriteRenderLayer : public CRenderLayer
{
public:
	CSpriteRenderLayer(int x, int y) : CRenderLayer(x, y) {};
	virtual ~CSpriteRenderLayer() {};

	virtual void Render() override;
	virtual void HandleEvents() override {};
	void AddSprite(std::weak_ptr<CSprite> pSprite);
	void RemoveSprite(std::weak_ptr<CSprite> pSprite);
private:
	std::vector<std::weak_ptr<CSprite>> m_aSprites; //ezen a struturan meg sokat lehetne optimalizalni

};
