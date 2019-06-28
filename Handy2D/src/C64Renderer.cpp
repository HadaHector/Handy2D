#pragma once
#include "C64Renderer.h"
#include "SDL.h"

namespace
{
	Color g_C64Colors[16] =
	{
		{0,0,0,255},		//0 black
		{255,255,255,255},	//1 white
		{136,0,0,255},		//2 red
		{170,255,238,255},	//3 cyan
		{204,68,204,255},	//4 purple
		{0,204,85,255},		//5 green
		{0,0,170,255},		//6 blue
		{238,238,119,255},  //7 yellow
		{221,136,85,255},	//8 orange
		{102,68,0,255},		//9 brown
		{255,119,119,255},  //10 pink
		{51,51,51,255},     //11 dark grey
		{119,119,119,255},  //12 grey
		{170,255,102,255},  //13 light green
		{0,136,255,255},    //14 light blue
		{187,187,187,255}   //15 light grey
	};

	SDL_Surface* g_pCharacters = nullptr;
}

CC64RenderLayer::CC64RenderLayer(const IntRect & rect) :
	CRenderLayer(rect),
	m_Painted(320, 200)
{
	auto pImage = std::make_shared<CImageSprite>();
	pImage->SetSize(rect.GetSize());
	pImage->SetTexture(CTexture::GetTexture(m_sName));
	m_pSprite = pImage;


	//load font bitmaps
	auto TexRef = CTexture::LoadTexture("resources/c64chars.bmp", "c64chars", true);
	if (TexRef)
	{
		auto pSurface = TexRef.lock()->GetSurface();
		if (pSurface)
		{
			g_pCharacters = pSurface;
		}
	}
}

void CC64RenderLayer::SetName(const std::string& sName)
{ 
	m_sName = sName;
	m_pSprite->SetTexture(CTexture::GetTexture(m_sName));
	m_bDirty = true;
}

SC64Char CC64RenderLayer::GetCharacter(int x, int y) const
{
	return m_aChars[x + y * 40];
}

void CC64RenderLayer::SetCharacter(int x, int y, const SC64Char & Char)
{
	m_aChars[x + y * 40] = Char;
	m_bDirty = true;

	DrawCharacter(x, y);
}

void CC64RenderLayer::SetPixel(int x, int y, bool bOn)
{
	int bigx = x / 8;
	int bigy = y / 8;
	m_aChars[bigx + bigy * 40].aPixels[(y - bigy*8) * 8 + x - bigx*8] = bOn;
	m_bDirty = true;
}


void CC64RenderLayer::Redraw()
{
	for (int x = 0; x < 40; ++x)
	{
		for (int y = 0; y < 25; ++y)
		{
			DrawCharacter(x, y);
		}
	}
}

void CC64RenderLayer::DrawCharacter(int x, int y)
{
	SC64Char& Char = m_aChars[x + y * 40];
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
			m_pSprite->SetTexture(m_Painted.CreateTexture(m_sName));
			m_bDirty = false;
		}

		m_pSprite->Render(*this);
	}
}

void SC64Char::SetCharacter(char iChar, bool bShifted)
{
	if (!g_pCharacters) return;

	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			aPixels[x + y * 8] = ((unsigned char*)(g_pCharacters->pixels))[8 * (iChar % 16) + x + (8 * (iChar / 16) + y + (bShifted?0:16*8)) * (16 * 8)] ? true : false;
		}
	}
}
