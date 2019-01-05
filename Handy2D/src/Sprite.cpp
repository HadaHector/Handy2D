#include "SDL.h"
#include "SDL_ttf.h"
#include "Sprite.h"
#include "SDLManager.h"
#include "Font.h"


//CSprite

SDL_Rect CSprite::GetRect()
{
	SDL_Rect Rect;
	Rect.x = (int)roundf(m_Position.x);
	Rect.y = (int)roundf(m_Position.y);
	Rect.w = (int)roundf(m_Size.x);
	Rect.h = (int)roundf(m_Size.y);
	return Rect;
}

//CImageSprite

CImageSprite::CImageSprite() { SDLManager::spritenum++; }

CImageSprite::~CImageSprite() { SDLManager::spritenum--; }

void CImageSprite::Render(const CRenderLayer& Layer, Vec vOffset)
{
	SDL_Rect Rect = GetRect();
	Rect.x += vOffset.x;
	Rect.y += vOffset.y;
	SDL_Rect Rect2 = Layer.GetRect();
	if (!SDL_HasIntersection(&Rect2, &Rect)) return;

	if (m_pTexture.expired()) return;
	auto pTextureShared = m_pTexture.lock();
	auto pData = pTextureShared.get()->GetTexture();
	IntVec vOriginalSize = pTextureShared->GetSize();

	SDL_Rect Result;
	SDL_IntersectRect(&Rect, &Rect2, &Result);

	SDL_Rect Src;
	Src.x = Rect.x < Rect2.x ? Rect2.x - Rect.x : 0;
	Src.y = Rect.y < Rect2.y ? Rect2.y - Rect.y : 0;
	Src.x *= vOriginalSize.x / m_Size.x;
	Src.y *= vOriginalSize.y / m_Size.y;
	Src.w = Result.w * (vOriginalSize.x / m_Size.x);
	Src.h = Result.h * (vOriginalSize.y / m_Size.y);

	SDL_RenderCopy(SDLManager::Instance.GetRenderer(), pData, &Src, &Result);
	SDLManager::drawnum++;
}

void CImageSprite::SetTexture(std::weak_ptr<CTexture> pTexture)
{
	m_pTexture = pTexture;
}

//CAnimSprite

CAnimSprite::~CAnimSprite() { SDLManager::spritenum--; }

void CAnimSprite::Render(const CRenderLayer& Layer, Vec vOffset)
{
	int nFrame = m_nFrame;
	if (m_frametime != 0.0f)
	{
		m_clock = fmod(m_clock + Time::frame, m_nRows*m_nCols*m_frametime);
		nFrame = (int)floor(m_clock / m_frametime);
	}
	int nRow = nFrame / m_nCols;
	int nCol = nFrame % m_nCols;

	if (m_pTexture.expired()) return;
	auto pTextureShared = m_pTexture.lock();
	auto pData = pTextureShared.get()->GetTexture();

	SDL_Rect FromRect;
	FromRect.x = (int) (nCol * m_TileSize.x);
	FromRect.y = (int) (nRow * m_TileSize.y);
	FromRect.w = (int) m_TileSize.x;
	FromRect.h = (int) m_TileSize.y;

	SDL_Rect Rect = GetRect();
	Rect.x += vOffset.x;
	Rect.y += vOffset.y;
	SDL_Rect Rect2 = Layer.GetRect();
	if (!SDL_HasIntersection(&Rect2, &Rect)) return;


	SDL_Rect Result;
	SDL_IntersectRect(&Rect, &Rect2, &Result);

	SDL_Rect Src;
	Src.x = Rect.x < Rect2.x ? Rect2.x - Rect.x : 0;
	Src.y = Rect.y < Rect2.y ? Rect2.y - Rect.y : 0;
	Src.x *= m_TileSize.x / m_Size.x;
	Src.y *= m_TileSize.y / m_Size.y;
	Src.x += FromRect.x;
	Src.y += FromRect.y;
	Src.w = FromRect.w * (Result.w / m_Size.x);
	Src.h = FromRect.h * (Result.h / m_Size.y);


	SDL_RenderCopy(SDLManager::Instance.GetRenderer(), pData, &Src, &Result);
	SDLManager::drawnum++;

}

void CAnimSprite::SetTexture(std::weak_ptr<CTexture> pTexture)
{
	CImageSprite::SetTexture(pTexture);
	RefreshTileSizes();
}

void CAnimSprite::RefreshTileSizes()
{
	if (m_pTexture.expired()) return;
	auto pTex = m_pTexture.lock();
	m_Size = pTex->GetSize();
	m_TileSize.x = m_Size.x / m_nCols;
	m_TileSize.y = m_Size.y / m_nRows;
}

//CTextSprite

void CTextSprite::Render(const CRenderLayer & Layer, Vec vOffset)
{
	if (!m_pFont) return;
	if (m_bCreateError) return;

	if (!m_pTextTexture || m_sLastText != m_sText)
	{
		SDL_Surface* pSurface = TTF_RenderText_Blended(m_pFont, m_sText.c_str(), m_Color);
		if (pSurface)
		{
			m_TextSize.x = (float) pSurface->w;
			m_TextSize.y = (float) pSurface->h;
			if (m_pTextTexture)
			{
				SDL_DestroyTexture(m_pTextTexture);
			}
			m_pTextTexture = SDL_CreateTextureFromSurface(SDLManager::Instance.GetRenderer(), pSurface);
			SDL_FreeSurface(pSurface);
		}
		else
		{
			m_bCreateError = true;
		}
	}

	SDL_Rect Rect = GetRect();
	//egyelore egyfele mod van: kozepre igazit, ha tul kicsi
	//majd kesobb lehetne tobb align
	if (Rect.w > m_TextSize.x)
	{
		Rect.x += (int) ((Rect.w - m_TextSize.x) * m_fAlignX);
		Rect.w = (int) m_TextSize.x;
	}
	if (Rect.h > m_TextSize.y)
	{
		Rect.y += (int)((Rect.h - m_TextSize.y) * m_fAlignY);
		Rect.h = (int) m_TextSize.y;
	}
	SDL_RenderCopy(SDLManager::Instance.GetRenderer(), m_pTextTexture, nullptr, &Rect);
	SDLManager::drawnum++;
}

CTextSprite::~CTextSprite()
{
	SDL_DestroyTexture(m_pTextTexture);
}

void CTextSprite::SetFont(const std::string& sFont, int nFontSize)
{
	m_sFont = sFont;
	m_nFontSize = nFontSize;
	m_pFont = FontCache::Instance.GetFont(sFont, nFontSize)->m_pFont;
}