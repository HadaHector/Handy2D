#pragma once
#include <string>

struct _TTF_Font;

class Font
{
	Font(const std::string& sPath);
	std::string m_sPath;
	_TTF_Font* m_pFont = nullptr;
};
