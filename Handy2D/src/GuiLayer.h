#pragma once
#include "Macros.h"
#include "RenderLayer.h"
#include "TextboxSprite.h"
#include <functional>

enum class EGuiResizeEffect
{
	None,
	Up,
	Down,
	Both
};

enum EHorizontalAlign {
	EHA_Left,
	EHA_Center,
	EHA_Right
};

enum EVerticalAlign {
	EVA_Top,
	EVA_Center,
	EVA_Bottom
};

class CGuiLayer;
class CGuiElement;

POINTER_TYPEDEFS(GuiElement)

struct SClickEvent
{
	bool m_bPropagation = true;
	IntVec m_vElemPos;
	IntVec m_vScreenPos;
	int m_nButton;
	WGuiElement m_pInitiator;
};

class CGuiElement : public std::enable_shared_from_this<CGuiElement>
{
public:
	CGuiElement();
	virtual EGuiResizeEffect GetResizeEffect() { return EGuiResizeEffect::None; }
	std::vector<PGuiElement>& GetChildren() { return m_aChildren; }

	void AddChild(PGuiElement elem);
	virtual void SetPosition(const IntVec& pos) { m_vPosition = pos; }
	IntVec GetPosition() const { return m_vPosition; }
	virtual void SetSize(const IntVec& size) { m_vSize = size; }
	IntVec GetSize() const { return m_vSize; }

	virtual void Render();
	void SetLayer(CGuiLayer* pLayer) { m_pLayer = pLayer; }

	int AddClickEventListener(std::function<void(SClickEvent&)> Event);
	void ClearClickEventListeners() { m_aClickEventHandlers.clear(); }
	void OnClick(SClickEvent& Event);
	std::map<std::string, std::string>& Properties() { return m_mProperties; }
	void SetVisible(bool bVis) { m_bVisible = bVis; }
	bool IsVisible() const { return m_bVisible; }
	bool IsClickable() const { return m_bClickable; }
	void SetClickable(bool bOn) { m_bClickable = bOn; }
protected:

	std::vector<PGuiElement> m_aChildren;
	bool m_bVisible = true;
	bool m_bClickable = false;
	IntVec m_vPosition;
	IntVec m_vSize;

	CGuiLayer* m_pLayer = nullptr;
	WGuiElement m_pParent;

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
	virtual void SetSize(const IntVec& size) { m_vSize = size; m_eValid = EUnknown;}
private:
	void CalcSize();

	std::string m_sImagePath;
	std::shared_ptr<CImageSprite> m_pSprite;

	EValidity m_eValid = EUnknown;

	IntVec m_vUsedSize;
};

POINTER_TYPEDEFS(GuiImage)

class CGuiText : public CGuiElement
{
public:
	CGuiText();
	void SetText(const std::string& sText);
	void SetFont(const std::string& sFont, int nFontSize);
	virtual void SetSize(const IntVec& size) override;
	virtual void Render();
	void SetAlign(EHorizontalAlign eHAlign, EVerticalAlign eVAlign);
	void SetColor(Color color);
private:

	void CheckValidity();

	std::string m_sText;
	std::string m_sFont;
	Color m_Color;
	int m_nFontSize = 8;
	std::shared_ptr<CTextSprite> m_pSprite;
	EHorizontalAlign m_eHAlign = EHA_Center;
	EVerticalAlign m_eVAlign = EVA_Center;

	EValidity m_eValid = EUnknown;
};

POINTER_TYPEDEFS(GuiText)

class CGuiTextbox : public CGuiElement
{
public:
	CGuiTextbox();
	
	virtual void Render() override;
	void Clear();
	void AddBlock(const STextBlock& Block);
	void SetDynamic(bool d) { m_pSprite->SetDynamic(d); }
	bool IsDynamic() const { return m_pSprite->IsDynamic(); }
	void SetAlign(EHorizontalAlign eHAlign, EVerticalAlign eVAlign);
	virtual void SetSize(const IntVec& size);
private:

	Color m_Color;
	int m_nFontSize = 8;
	std::shared_ptr<CTextboxSprite> m_pSprite;
	EValidity m_eValid = EUnknown;
};

POINTER_TYPEDEFS(GuiTextbox)

struct SAlignSettings
{
	IntVec size;

};

struct ElementRect
{
	ElementRect() {};
	IntRect rect;
	WGuiElement element;
};

class CGuiLayer :
	public CRenderLayer
{
public:
	CGuiLayer(const IntRect& Rect);
	~CGuiLayer();

	void Render();
	virtual void HandleEvents() override;
	void AddRenderData(std::weak_ptr<CSprite> pSprite, WGuiElement pElem);
	PGuiElement GetRootElement() { return m_pRoot; }

	bool IsCursorAboveElement() const { return m_bCursorAboveElement; }
private:
	PGuiElement m_pRoot;

	std::vector<std::weak_ptr<CSprite>> m_aRenderArray;
	std::vector<ElementRect> m_aRects;

	bool m_bCursorAboveElement = false;
};

POINTER_TYPEDEFS(GuiLayer)

