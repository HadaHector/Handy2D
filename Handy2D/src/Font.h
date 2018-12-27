#pragma once
#include <string>
#include <map>
#include "SDL.h"

struct _TTF_Font;
struct FontDesc;

struct LetterDesc
{
	Uint16 character;
	int maxx = 0, minx = 0, maxy = 0, miny = 0, advance = 0;
	SDL_Surface* pSurface = nullptr;
	SDL_Texture* pTexture = nullptr;
	FontDesc* pFont;
	bool bWhiteSpace = false;
};


struct FontDesc
{
	LetterDesc* GetLetter(Uint16 ch);
	std::string m_sFontPath;
	_TTF_Font* m_pFont;
	std::map<int, LetterDesc*> m_mLetters;

	int m_nAscend, m_nDescend, m_nLineSkip;
};


class FontCache
{
public:
	static FontCache Instance;
	FontDesc* GetFont(const std::string& sFont, int nSize);
private:
	FontCache() {};
	~FontCache();
	
	std::map < std::string, std::map<int, FontDesc>> m_mFonts;
};

