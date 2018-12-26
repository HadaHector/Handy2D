#pragma once
#include <string>
#include <map>

struct _TTF_Font;

struct FontDesc
{
	std::string m_sFontPath;
	_TTF_Font* m_pFont;
};


class FontCache
{
public:
	static FontCache Instance;
	_TTF_Font* GetFont(const std::string& sFont, int nSize);
private:
	FontCache() {};
	~FontCache();
	
	std::map < std::string, std::map<int, FontDesc>> m_mFonts;
};
