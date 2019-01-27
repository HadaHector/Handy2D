#include "GuiLayer.h"
#include "Input.h"
#include "SDL.h"


CGuiElement::CGuiElement() :
	m_vPosition(0, 0), m_vSize(-1, -1) {};

void CGuiElement::Render()
{
	if (m_bVisible)
	{
		for (CGuiElement* elem : m_aChildren)
		{
			elem->Render();
		}
	}
}

void CGuiElement::AddChild(CGuiElement* elem)
{
	elem->m_pLayer = m_pLayer;
	elem->m_pParent = this;
	m_aChildren.push_back(elem);
}

void CGuiElement::OnClick(SClickEvent& Event)
{
	for (auto&& func : m_aClickEventHandlers)
	{
		func(Event);
	}

	if (Event.m_bPropagation && m_pParent)
	{
		m_pParent->OnClick(Event);
	}
}

int CGuiElement::AddClickEventListener(std::function<void(SClickEvent&)> Event)
{
	m_aClickEventHandlers.push_back(Event);
	return m_aClickEventHandlers.size() - 1;
}


CGuiLayer::CGuiLayer(const IntRect& Rect) : CRenderLayer(Rect)
{
	m_pRoot = new CGuiElement();
	m_pRoot->SetLayer(this);
}


CGuiLayer::~CGuiLayer()
{
}

void CGuiLayer::Render()
{
	m_aRenderArray.clear();
	m_pRoot->Render();

	for (auto&& Sprite : m_aRenderArray)
	{
		Sprite.lock()->Render(*this);
	}
}

void CGuiLayer::HandleEvents()
{
	IntVec mouse = Input::GetMousePos(); 
	for (int i=m_aRects.size()-1; i>=0; --i)
	{
		IntRect& rect = m_aRects[i].rect;
		if (rect.GetUpperLeft().x <= mouse.x && rect.GetUpperLeft().y <= mouse.y &&
			rect.GetBottomRight().x >= mouse.x && rect.GetBottomRight().y >= mouse.y)
		{
			int nButton = 0;
			if (Input::GetKey(MOUSE_LEFT).released)
			{
				nButton = MOUSE_LEFT;
			}
			else if (Input::GetKey(MOUSE_RIGHT).released)
			{
				nButton = MOUSE_RIGHT;
			}
			else if (Input::GetKey(MOUSE_MIDDLE).released)
			{
				nButton = MOUSE_MIDDLE;
			}

			if (nButton != 0)
			{
				SClickEvent Event;
				Event.m_nButton = nButton;
				Event.m_vScreenPos = mouse;
				Event.m_vElemPos = mouse - rect.GetUpperLeft();
				Event.m_pInitiator = m_aRects[i].element;
				m_aRects[i].element->OnClick(Event);
			}

			break;
		}
	}

	m_aRects.clear();
}

void CGuiLayer::AddRenderData(std::weak_ptr<CSprite> pSprite, CGuiElement* pElem)
{
	m_aRenderArray.push_back(pSprite);
	ElementRect Rect;
	Rect.rect = pSprite.lock()->GetRect();
	Rect.element = pElem;
	m_aRects.push_back(Rect);
}

CGuiImage::CGuiImage() : CGuiElement(),
	m_vUsedSize(-1, -1) {};

void CGuiImage::SetImage(const std::string& sPath)
{
	m_sImagePath = sPath;
	m_pSprite = std::make_shared<CImageSprite>();
	m_pSprite->SetTexture(CTexture::GetTexture(sPath));
	m_eValid = EUnknown;
}

void CGuiImage::CalcSize()
{
	if (m_vSize.x >= 0) m_vUsedSize.x = m_vSize.x;
	if (m_vSize.y >= 0) m_vUsedSize.y = m_vSize.y;
	if (m_vUsedSize.x == -1 || m_vUsedSize.y == -1)
	{
		std::shared_ptr<CTexture> pTex = CTexture::GetTexture(m_sImagePath).lock();
		if (pTex)
		{
			IntVec vTexSize = pTex->GetSize();
			if (vTexSize.x <= 0 || vTexSize.y <= 0)
			{
				m_eValid = EInvalid;
				return;
			}

			if (m_vUsedSize.x == -1 && m_vUsedSize.y == -1)
			{
				m_vUsedSize = vTexSize;
			}
			else if (m_vUsedSize.x == -1)
			{
				m_vUsedSize.x = m_vUsedSize.y * vTexSize.x / vTexSize.y;
			}
			else
			{
				m_vUsedSize.y = m_vUsedSize.x * vTexSize.y / vTexSize.x;
			}
		}
		else
		{
			m_eValid = EInvalid;
			return;
		}
	}

	if (m_vUsedSize.x <= 0 || m_vUsedSize.y <= 0)
	{
		m_eValid = EInvalid;
		return;
	}

	m_eValid = EValid;

}

void CGuiImage::Render()
{
	if (!m_bVisible) return;

	if (m_pSprite)
	{

		if (m_eValid == EUnknown)
		{
			CalcSize();
		}

		if (m_eValid == EInvalid) return;

		m_pSprite->SetPos(m_vPosition);
		m_pSprite->SetSize(m_vUsedSize);
		m_pLayer->AddRenderData(m_pSprite, this);
	}

	CGuiElement::Render();
}

CGuiText::CGuiText() : CGuiElement()
{
	m_vSize = IntVec(-1, -1);
	m_pSprite = std::make_shared<CTextSprite>();
};


void CGuiText::SetText(const std::string& sText)
{
	m_sText = sText;
	m_pSprite->SetText(sText);
}

void CGuiText::SetSize(const IntVec& size)
{
	if (m_vSize == size) return;
	m_vSize = size;
	m_eValid = EUnknown;
}

void CGuiText::CheckValidity()
{
	if (m_vSize.x > 0 && m_vSize.y > 0 && m_sText.size() > 0 && m_sFont.size() > 0)
	{
		m_eValid = EValidity::EValid;
	}
	else
	{
		m_eValid = EValidity::EInvalid;
	}
}

void CGuiText::SetFont(const std::string& sFont, int nFontSize)
{
	m_sFont = sFont;
	m_nFontSize = nFontSize;
	m_pSprite->SetFont(sFont, nFontSize);
}

void CGuiText::Render()
{
	if (m_pSprite)
	{

		if (m_eValid == EUnknown)
		{
			CheckValidity();
		}

		if (m_eValid == EInvalid) return;

		m_pSprite->SetPos(m_vPosition);
		m_pSprite->SetSize(m_vSize);
		m_pLayer->AddRenderData(m_pSprite, this);
	}

	CGuiElement::Render();
}

void CGuiText::SetAlign(EHorizontalAlign eHAlign, EVerticalAlign eVAlign)
{
	m_eHAlign = eHAlign;
	m_eVAlign = eVAlign;
	float f;

	switch (m_eHAlign)
	{
	case EHA_Left: f = 0.0f; break;
	case EHA_Center: f = 0.5f; break;
	case EHA_Right: f = 1.0f; break;
	}

	m_pSprite->SetAlignX(f);

	switch (m_eVAlign)
	{
	case EVA_Bottom: f = 1.0f; break;
	case EVA_Center: f = 0.5f; break;
	case EVA_Top: f = 0.0f; break;
	}

	m_pSprite->SetAlignY(f);
}

void CGuiText::SetColor(Color color)
{
	m_Color = color;
	m_pSprite->SetColor(color);
}


CGuiTextbox::CGuiTextbox()
{
	m_vSize = IntVec(10, 10);
	m_pSprite = std::make_shared<CTextboxSprite>();
}

void CGuiTextbox::Render()
{
	if (m_pSprite)
	{

		m_pSprite->SetPos(m_vPosition);
		m_pSprite->SetSize(m_vSize);
		m_pLayer->AddRenderData(m_pSprite, this);
	}

	CGuiElement::Render();
}


void CGuiTextbox::AddBlock(const STextBlock& Block)
{
	m_pSprite->AddBlock(Block);
}

void CGuiTextbox::Clear()
{
	m_pSprite->Clear();
}

void CGuiTextbox::SetAlign(EHorizontalAlign eHAlign, EVerticalAlign eVAlign)
{
	float f;

	switch (eHAlign)
	{
	case EHA_Left: f = 0.0f; break;
	case EHA_Center: f = 0.5f; break;
	case EHA_Right: f = 1.0f; break;
	}

	m_pSprite->SetHAlign(f);

	switch (eVAlign)
	{
	case EVA_Bottom: f = 1.0f; break;
	case EVA_Center: f = 0.5f; break;
	case EVA_Top: f = 0.0f; break;
	}

	m_pSprite->SetVAlign(f);
	m_pSprite->Invalidate();
}

void CGuiTextbox::SetSize(const IntVec& size)
{ 
	if (m_vSize == size) return;
	m_vSize = size; 
	m_eValid = EUnknown; 
	m_pSprite->Invalidate();
}