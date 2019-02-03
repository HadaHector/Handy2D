#pragma once
#include <map>
#include <string>
#include "Geometry.h"
#include <memory>
#include <functional>

struct SDL_Texture;
struct SDL_Surface;
struct STextureRef;

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
	size_t m_hash;

	void Load();
public:

	CTexture(); //do not use!
	virtual ~CTexture();

	static STextureRef LoadTexture(const std::string& sFilePath, const std::string& sName = "");
	static STextureRef AddSurface(SDL_Surface* pSurface, const std::string& sName);
	static void UnloadTextures();
	static STextureRef GetTexture(const std::string& sName);
	static bool DelTexture(const std::string& sName);
	static std::map<std::string, std::shared_ptr<CTexture>> m_mStore;

	SDL_Texture* GetTexture();
	size_t GetHash() const { return m_hash; }
	IntVec GetSize() const { return m_Size; }
	
};

struct STextureRef
{
	std::weak_ptr<CTexture> m_tex;
	size_t m_hash = 0;
	STextureRef() {}
	STextureRef(std::weak_ptr<CTexture> tex)
	{
		m_tex = tex;
		if (!tex.expired())
		{
			m_hash = tex.lock()->GetHash();
		}
		else
		{
			m_hash = 0;
		}
	}
	STextureRef(std::shared_ptr<CTexture> tex)
	{
		m_tex = tex;
		m_hash = tex->GetHash();
	}

	bool operator==(const STextureRef& other) const
	{
		return m_hash == other.m_hash;
	}

	operator std::weak_ptr<CTexture>() {
		return m_tex;
	}

	operator bool() const {
		return m_hash != 0 && !m_tex.expired();
	}

	std::shared_ptr<CTexture> lock() {
		return m_tex.lock();
	}
};
