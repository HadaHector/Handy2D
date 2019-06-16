#pragma once
#include "C64Renderer.h"


CC64RenderLayer::CC64RenderLayer(const IntRect & rect) :
	CRenderLayer(rect),
	m_Painted(320, 200)
{
	auto pImage = std::make_shared<CImageSprite>();
	pImage->SetSize(rect.GetSize());
	pImage->SetTexture(CTexture::GetTexture(m_sName));
	m_pSprite = pImage;
}

void CC64RenderLayer::SetName(const std::string& sName)
{ 
	m_sName = sName;
	m_pSprite.lock()->SetTexture(CTexture::GetTexture(m_sName));
	m_bDirty = true;
}

SC64Char CC64RenderLayer::GetCharacter(int x, int y) const
{
	return m_aChars[x + y * 40];
}

void CC64RenderLayer::SetCharacter(int x, int y, const SC64Char & Char)
{
	m_aChars[x + y * 25] = Char;
	m_bDirty = true;

	DrawCharacter(x, y);
}

void CC64RenderLayer::DrawCharacter(int x, int y)
{
	SC64Char& Char = m_aChars[x + y * 16];
	for (char i = 0; i < 8; ++i)
	{
		for (char j = 0; j < 8; ++j)
		{
			m_Painted.Set(x * 8 + i, y * 8 + j, g_C64Colors[(int)(Char.aPixels[i + j * 8] ? Char.cFG : Char.cBG)]);
		}
	}
}

void CC64RenderLayer::Render()
{
	if (m_bVisible)
	{
		if (m_bDirty)
		{
			m_Painted.CreateTexture(m_sName);
		}

		m_pSprite.lock()->Render(*this);
	}
}
