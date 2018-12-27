#include <string>
#include "TextboxSprite.h"
#include <algorithm>
#include "sdlmanager.h"
#include "Sprite.h"
#include "Font.h"

std::wstring UTF8toUnicode(const std::string& s)
{
	std::wstring ws;
	wchar_t wc;
	for (unsigned int i = 0; i < s.length(); )
	{
		char c = s[i];
		if ((c & 0x80) == 0)
		{
			wc = c;
			++i;
		}
		else if ((c & 0xE0) == 0xC0)
		{
			wc = (s[i] & 0x1F) << 6;
			wc |= (s[i + 1] & 0x3F);
			i += 2;
		}
		else if ((c & 0xF0) == 0xE0)
		{
			wc = (s[i] & 0xF) << 12;
			wc |= (s[i + 1] & 0x3F) << 6;
			wc |= (s[i + 2] & 0x3F);
			i += 3;
		}
		else if ((c & 0xF8) == 0xF0)
		{
			wc = (s[i] & 0x7) << 18;
			wc |= (s[i + 1] & 0x3F) << 12;
			wc |= (s[i + 2] & 0x3F) << 6;
			wc |= (s[i + 3] & 0x3F);
			i += 4;
		}
		else if ((c & 0xFC) == 0xF8)
		{
			wc = (s[i] & 0x3) << 24;
			wc |= (s[i] & 0x3F) << 18;
			wc |= (s[i] & 0x3F) << 12;
			wc |= (s[i] & 0x3F) << 6;
			wc |= (s[i] & 0x3F);
			i += 5;
		}
		else if ((c & 0xFE) == 0xFC)
		{
			wc = (s[i] & 0x1) << 30;
			wc |= (s[i] & 0x3F) << 24;
			wc |= (s[i] & 0x3F) << 18;
			wc |= (s[i] & 0x3F) << 12;
			wc |= (s[i] & 0x3F) << 6;
			wc |= (s[i] & 0x3F);
			i += 6;
		}
		ws += wc;
	}
	return ws;
}

FontDesc* STextBlock::GetFont()
{
	if (pFont) return pFont;
	pFont = FontCache::Instance.GetFont(sFont, nFontSize);
	return pFont;
}

void STextBlock::SetText(const std::string& sText)
{
	this->sText = sText;
	sWText = UTF8toUnicode(sText);
}
void STextBlock::SetFont(const std::string& sFont)
{
	this->sFont = sFont;
}
void STextBlock::SetFontSize(int nSize)
{
	this->nFontSize = nSize;
}
void STextBlock::SetColor(Color color)
{
	this->color = color;
}


void CTextboxSprite::Render(const CRenderLayer& Layer)
{
	if (!m_bGenerated)
	{
		//first we collect the rects to render, then deal with line height, line break, align etc
		std::vector<LetterDesc*> aLetters;
		std::vector<Color> aColors;
		for (unsigned int i = 0; i < m_aBlocks.size(); ++i)
		{
			for (unsigned int c = 0; c < m_aBlocks[i].sWText.size(); ++c)
			{
				LetterDesc* desc;
				desc = m_aBlocks[i].GetFont()->GetLetter(m_aBlocks[i].sWText[c]);
				aLetters.push_back(desc);
				aColors.push_back(m_aBlocks[i].color);
			}
		}

		m_aGlyphs.clear();
		m_aGlyphs.reserve(aLetters.size());

		{
			int nWordStart = 0;
			int nLine = 0;
			int nX = 0;
			bool bFullLine = true;

			for (unsigned int i = 0; i < aLetters.size(); ++i)
			{
				LetterDesc* letter = aLetters[i];
				renderglyph Glyph;
				Glyph.pSurface = letter->pSurface;
				Glyph.pTexture = letter->pTexture;
				Glyph.color = aColors[i];


				if (letter->character == '\n')
				{
					Glyph.line = nLine;
					nLine++;
					nX = 0;
				}
				else
				{
					if (letter->character == ' ')
					{
						Glyph.line = nLine;
						nWordStart = i + 1;
						bFullLine = false;

						if (nX + letter->advance >= m_Size.x)
						{
							nLine++;
							nX = 0;
						}
						else if (nX > 0)
						{
							nX += letter->advance;
						}
					}
					else
						if (nX + letter->advance >= m_Size.x)
						{
							if (!bFullLine)
							{
								nLine++;
								nX = 0;
								for (unsigned int j = nWordStart; j < i; ++j)
								{
									m_aGlyphs[j].line = nLine;
									m_aGlyphs[j].x = nX;
									nX += aLetters[j]->advance;
								}
								Glyph.line = nLine;
								Glyph.x = nX;
								nX += letter->advance;
							}
							else //fulline - no space in row, too long word, must be broke
							{
								nLine++;
								nWordStart = i;
								Glyph.line = nLine;
								Glyph.x = 0;
								nX = letter->advance;
							}
						}
						else
						{
							Glyph.line = nLine;
							Glyph.x = nX;
							nX += letter->advance;
						}
				}
				m_aGlyphs.push_back(Glyph);
			}
		}

		{
			int nLine = 0;
			int nLineStart = 0;
			int nLineY = 0;
			int nLineMaxAscend = 0;
			int nLineMaxSkip = 0;

			int nMaxVertical = 0;

			for (unsigned int i = 0; i < aLetters.size(); ++i)
			{
				LetterDesc* letter = aLetters[i];
				renderglyph Glyph = m_aGlyphs[i];

				if (nLine != Glyph.line || i == aLetters.size() - 1)
				{
					if (i == aLetters.size() - 1) i++; //hack for the last character
					int nOffset = (int) (m_fHorizontalAlign * (m_Size.x - m_aGlyphs[i - 1].x));

					for (unsigned int j = nLineStart; j < i; ++j)
					{
						m_aGlyphs[j].y = nLineY + nLineMaxAscend - aLetters[j]->pFont->m_nAscend;
						m_aGlyphs[j].x += nOffset;
						nMaxVertical = std::max(nMaxVertical, m_aGlyphs[j].y + letter->pFont->m_nDescend);
					}
					nLineY += nLineMaxSkip;
					nLineStart = i;
					nLineMaxAscend = 0, nLineMaxSkip = 0;
					nLine++;
				}

				nLineMaxAscend = std::max(nLineMaxAscend, letter->pFont->m_nAscend);
				nLineMaxSkip = std::max(nLineMaxSkip, letter->pFont->m_nLineSkip);
			}


			int nOffset = (int) (m_fVerticalAlign * ( m_Size.y - nMaxVertical));
			for (unsigned int i = 0; i < m_aGlyphs.size(); ++i)
			{
				m_aGlyphs[i].y += nOffset;
			}
		}
		m_bGenerated = true;
	}

	if (m_bDynamic)
	{
		for (unsigned int i = 0; i < m_aGlyphs.size(); ++i)
		{
			//special characters may not have surface, for example \n
			if (!m_aGlyphs[i].pSurface || !m_aGlyphs[i].pTexture) continue;
			SDL_Rect Rect2;
			Rect2 = m_aGlyphs[i].pSurface->clip_rect;
			Rect2.x += (int) m_Position.x + m_aGlyphs[i].x;
			Rect2.y += (int) m_Position.y + m_aGlyphs[i].y;

			Color& c = m_aGlyphs[i].color;
			int ret = SDL_SetTextureColorMod(m_aGlyphs[i].pTexture, c.r, c.g, c.b);

			//todo clip the overflowing stuff
			
			SDL_RenderCopy(SDLManager::Instance.GetRenderer(), m_aGlyphs[i].pTexture, nullptr, &Rect2);
		}
	}
	else
	{

		SDL_Surface* pTarget = SDL_CreateRGBSurface(0, (int) m_Size.x, (int) m_Size.y, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
		for (unsigned int i = 0; i < m_aGlyphs.size(); ++i)
		{
			//special characters may not have surface, for example \n
			if (!m_aGlyphs[i].pSurface) continue;
			SDL_Rect Rect2;
			Rect2 = m_aGlyphs[i].pSurface->clip_rect;
			Rect2.x += m_aGlyphs[i].x;
			Rect2.y += m_aGlyphs[i].y;

			//todo not necessary to multiply if the color is white
			SDL_SetSurfaceBlendMode(pTarget, SDL_BLENDMODE_NONE);
			Uint32 col = m_aGlyphs[i].color.forSurface(pTarget);
			col &= 0xffffff00;
			SDL_FillRect(pTarget, &Rect2, col);

			SDL_SetSurfaceBlendMode(pTarget, SDL_BLENDMODE_MOD);
			SDL_BlitSurface(m_aGlyphs[i].pSurface, NULL, pTarget, &Rect2);

		}

		if (m_pTextTexture)
		{
			SDL_DestroyTexture(m_pTextTexture);
		}
		m_pTextTexture = SDL_CreateTextureFromSurface(SDLManager::Instance.GetRenderer(), pTarget);
		SDL_FreeSurface(pTarget);

		SDL_Rect Rect;
		Rect.x = (int) m_Position.x;
		Rect.y = (int) m_Position.y;
		Rect.w = (int) m_Size.x;
		Rect.h = (int) m_Size.y;
		SDL_RenderCopy(SDLManager::Instance.GetRenderer(), m_pTextTexture, nullptr, &Rect);
	}
	SDLManager::drawnum++;
	
}

void CTextboxSprite::SetHAlign(float fAlign)
{
	if (m_fHorizontalAlign != fAlign)
	{
		m_fHorizontalAlign = fAlign;
		m_bGenerated = false;
	}
}

void CTextboxSprite::SetVAlign(float fAlign)
{
	if (m_fVerticalAlign != fAlign)
	{
		m_fVerticalAlign = fAlign;
		m_bGenerated = false;
	}
}