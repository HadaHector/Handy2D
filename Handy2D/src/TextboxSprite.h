#pragma once
#include "Color.h"
#include "Sprite.h"

struct FontDesc;

struct STextBlock
{
	void SetText(const std::string& sText);
	void SetFont(const std::string& sFont);
	void SetFontSize(int nSize);
	void SetColor(Color color);

	std::string sText; //UTF8
	std::wstring sWText; //UNICODE
	Color color;
	std::string sFont;
	int nFontSize;
	FontDesc* pFont = nullptr;
	FontDesc* GetFont();
};

struct renderglyph
{
	int x, y;
	SDL_Surface* pSurface;
	SDL_Texture* pTexture;
	Color color;
	int line = 0;
};


class CTextboxSprite : public CSprite
{
public:
	CTextboxSprite() { m_bDynamic = true;  };
	void Clear() { m_aBlocks.clear(); }
	void AddBlock(const STextBlock& Block) { m_aBlocks.push_back(Block); m_bGenerated = false;}
	virtual void Render(const CRenderLayer& Layer, Vec vOffset = Vec()) override;
	void SetDynamic(bool d) { m_bDynamic = d; }
	bool IsDynamic() const { return m_bDynamic; }
	void SetHAlign(float fAlign);
	void SetVAlign(float fAlign);
	void Invalidate() { m_bGenerated = false; }
private:

	std::vector<STextBlock> m_aBlocks;
	struct SDL_Texture* m_pTextTexture = nullptr;
	bool m_bDynamic = false;
	bool m_bWordWrap = true;
	std::vector<renderglyph> m_aGlyphs;
	bool m_bGenerated = false;

	float m_fHorizontalAlign = 0.5f;
	float m_fVerticalAlign = 0.5f;
};