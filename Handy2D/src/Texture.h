#pragma once
#include <map>
#include <string>
#include "Geometry.h"
#include <memory>

struct SDL_Texture;
struct SDL_Surface;

class CTexture
{
private:
	SDL_Texture* m_pTexture = nullptr;
	IntVec m_Size;

	std::string m_sFilePath;
	std::string m_sName;
	bool m_bLoaded = false;
	bool m_bError = false;
	std::string m_sError;

	void Load();
public:

	CTexture(); //do not use!
	virtual ~CTexture();

	static std::weak_ptr<CTexture> LoadTexture(const std::string& sFilePath, const std::string& sName = "");
	static std::weak_ptr<CTexture> AddSurface(SDL_Surface* pSurface, const std::string& sName);
	static void UnloadTextures();
	static std::weak_ptr<CTexture> GetTexture(const std::string& sName);
	static bool DelTexture(const std::string& sName);
	static std::map<std::string, std::shared_ptr<CTexture>> m_mStore;

	SDL_Texture* GetTexture();
	IntVec GetSize() { return m_Size; }
	
};

