#include "Font.h"
#include "SDL_ttf.h"
#include <assert.h>

#if defined(_WIN32) || defined(_WIN64)
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#include <shlobj.h>
#include <comutil.h>
#include <windows.h>
#include <fstream>
#pragma comment(lib, "comsuppw")
bool fexists(const char *filename) {
	std::ifstream ifile(filename);
	return (bool)ifile;
}
#endif

FontCache FontCache::Instance;

_TTF_Font* FontCache::GetFont(const std::string& sFont, int nSize)
{
	auto it = m_mFonts.find(sFont);
	if (it != m_mFonts.end())
	{
		auto it2 = it->second.find(nSize);
		if (it2 != it->second.end())
		{
			return it2->second.m_pFont;
		}
	}
	
	std::string sPath = sFont;
	_TTF_Font* pFont = nullptr;
	#if defined(_WIN32) || defined(_WIN64)
	if (fexists(sFont.c_str()))
	{
		pFont = TTF_OpenFont(sFont.c_str(), nSize);
	}
	else
	{
		LPWSTR fontpath;
		HRESULT res = SHGetKnownFolderPath(FOLDERID_Fonts, 0, 0, &fontpath);
		_bstr_t bstrPath(fontpath);
		std::string sWinPath = ((char*)bstrPath);
		sWinPath += "\\" + sFont;
		if (fexists(sWinPath.c_str()))
		{
			pFont = TTF_OpenFont(sWinPath.c_str(), nSize);
			sPath = sWinPath;
		}
	}
	#else
		pFont = TTF_OpenFont(sFont.c_str(), nSize);
	#endif

	
	
	if (pFont)
	{
		if (it == m_mFonts.end())
		{
			auto pair = m_mFonts.insert(std::make_pair(sFont, std::map<int, FontDesc>()));
			assert(pair.second);
			it = pair.first;	
		}
		
		printf("Font loaded: %s %d \n", sFont.c_str(), nSize);
		FontDesc Desc;
		Desc.m_pFont = pFont;
		Desc.m_sFontPath = sPath;
		it->second.insert(std::make_pair(nSize, Desc));
		return pFont;
	}
	else
	{
		printf("Failed to load %s font! SDL_ttf Error: %s\n", sFont.c_str(), TTF_GetError());
	}

	return nullptr;
}

FontCache::~FontCache()
{
	/*
	for (auto it = m_mFonts.begin(); it != m_mFonts.end(); ++it)
	{
		std::map<int, FontDesc>& pMap = it->second;
		for (auto it2 = pMap.begin(); it2 != pMap.end(); ++it2)
		{
			TTF_CloseFont(it2->second.m_pFont);
		}
	}
	*/
	//the default destructors for the maps are enough
}

