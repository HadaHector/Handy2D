#pragma once
#include "Sprite.h"
#include <vector>
#include <memory>

class CSprite;

class CRenderLayer
{
protected:
	bool m_bVisible = true;
	IntRect m_Rect;
	std::vector<std::string> m_aTags;
public:
	CRenderLayer(const IntRect& rect) { m_Rect = rect; }
	virtual ~CRenderLayer() {};
	virtual void Render() = 0;
	virtual void HandleEvents() = 0;
	void SetVisible(bool bVisible) { m_bVisible = bVisible; }
	bool IsVisible() const { return m_bVisible; }
	IntRect GetRect() const { return m_Rect; }
	void SetRect(const IntRect& rect) { m_Rect = rect; }
	void AddTag(const std::string& sTag) { m_aTags.push_back(sTag);}
	bool HasTag(const std::string& sTag) const { for (auto&& tag : m_aTags) { if (tag == sTag) return true; } return false; }
};

class CSpriteRenderLayer : public CRenderLayer
{
public:
	CSpriteRenderLayer(const IntRect& rect) : CRenderLayer(rect) {};
	virtual ~CSpriteRenderLayer() {};

	virtual void Render() override;
	virtual void HandleEvents() override {};
	void AddSprite(std::weak_ptr<CSprite> pSprite);
	void RemoveSprite(std::weak_ptr<CSprite> pSprite);
protected:
	std::vector<std::weak_ptr<CSprite>> m_aSprites; //ezen a struturan meg sokat lehetne optimalizalni

};

class CCameraRenderLayer : public CSpriteRenderLayer
{
public:
	CCameraRenderLayer(const IntRect& rect) : CSpriteRenderLayer(rect) { m_vPosition = rect.GetUpperLeft(); };
	virtual ~CCameraRenderLayer() {};
	virtual void Render() override;
	void SetPosition(IntVec pos) { m_vPosition = pos; }
	IntVec GetPosition() { return m_vPosition; }
private:
	IntVec m_vPosition;
};