#pragma once
#include "RenderLayer.h"
#include <functional>

enum class EGuiResizeEffect
{
	None,
	Up,
	Down,
	Both
};

class CGuiLayer;
class CGuiElement;

struct SClickEvent
{
	bool m_bPropagation = true;
	IntVec m_vElemPos;
	IntVec m_vScreenPos;
	int m_nButton;
	CGuiElement* m_pInitiator = nullptr;
};

class CGuiElement
{
public:
	CGuiElement();
	virtual EGuiResizeEffect GetResizeEffect() { return EGuiResizeEffect::None; }
	std::vector<CGuiElement*>& GetChildren() { return m_aChildren; }

	void AddChild(CGuiElement* elem);
	void SetPosition(const IntVec& pos) { m_vPosition = pos; }
	IntVec GetPosition() { return m_vPosition; }

	virtual void Render();
	void SetLayer(CGuiLayer* pLayer) { m_pLayer = pLayer; }

	int AddClickEventListener(std::function<void(SClickEvent&)> Event);
	void ClearClickEventListeners() { m_aClickEventHandlers.clear(); }
	void OnClick(SClickEvent& Event);
	std::map<std::string, std::string>& Properties() { return m_mProperties; }
protected:

	std::vector<CGuiElement*> m_aChildren;
	bool m_bVisible = true;
	IntVec m_vPosition;
	IntVec m_vSize;

	CGuiLayer* m_pLayer = nullptr;
	CGuiElement* m_pParent = nullptr;

	std::map<std::string, std::string> m_mProperties;

	std::vector<std::function<void(SClickEvent&)>> m_aClickEventHandlers;
};

enum EValidity
{
	EUnknown,
	EValid,
	EInvalid
};

class CGuiImage : public CGuiElement
{
public:
	CGuiImage();
	void SetImage(const std::string& sPath);
	virtual void Render();
private:
	void CalcSize();

	std::string m_sImagePath;
	std::shared_ptr<CImageSprite> m_pSprite;

	EValidity m_eValid = EUnknown;

	IntVec m_vUsedSize;
};


struct ElementRect
{
	ElementRect() {};
	IntRect rect;
	CGuiElement* element;
};

class CGuiLayer :
	public CRenderLayer
{
public:
	CGuiLayer(int x, int y);
	~CGuiLayer();

	void Render();
	virtual void HandleEvents() override;
	void AddRenderData(std::weak_ptr<CSprite> pSprite, CGuiElement* pElem);
	CGuiElement* GetRootElement() { return m_pRoot; }
private:
	CGuiElement* m_pRoot;

	std::vector<std::weak_ptr<CSprite>> m_aRenderArray;
	std::vector<ElementRect> m_aRects;
};

