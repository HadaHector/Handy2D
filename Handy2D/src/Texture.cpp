#include "Texture.h"
#include <SDL.h>
#include <SDL_image.h>
#include "SDLManager.h"


std::map<std::string, std::shared_ptr<CTexture>> CTexture::m_mStore;

CTexture::CTexture()
{
	m_hash = std::hash<std::string>{}(m_sName);
}

CTexture::~CTexture()
{
	if (m_pTexture)
	{
		SDL_DestroyTexture(m_pTexture);
	}
}


SDL_Texture* CTexture::GetTexture()
{
	return m_pTexture;
}

SDL_Surface* CTexture::GetSurface()
{
	return m_pSurface;
}

Color CTexture::GetPixel(IntVec vCoord)
{
	if (!m_pSurface)
	{
		return Color();
	}
	unsigned char r = ((unsigned char*)m_pSurface->pixels)[4*(vCoord.y*m_pSurface->w + vCoord.x)];
	unsigned char g = ((unsigned char*)m_pSurface->pixels)[4*(vCoord.y*m_pSurface->w + vCoord.x)+1];
	unsigned char b = ((unsigned char*)m_pSurface->pixels)[4*(vCoord.y*m_pSurface->w + vCoord.x)+2];
	unsigned char a = ((unsigned char*)m_pSurface->pixels)[4*(vCoord.y*m_pSurface->w + vCoord.x)+3];
	return Color(r, g, b, a);
}

void CTexture::Load(bool bKeepSurface)
{
	m_pSurface = IMG_Load(m_sFilePath.c_str());
	if (m_pSurface == nullptr)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", m_sFilePath.c_str(), IMG_GetError());
		m_bError = true;
		m_sError = IMG_GetError();
	}
	else
	{
		//Create texture from surface pixels
		m_pTexture = SDL_CreateTextureFromSurface(SDLManager::Instance.GetRenderer(), m_pSurface);
		if (m_pTexture == nullptr)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", m_sFilePath.c_str(), SDL_GetError());
			m_bError = true;
			m_sError = SDL_GetError();
		}
		else
		{
			m_Size.x = m_pSurface->w;
			m_Size.y = m_pSurface->h;
			m_bLoaded = true;
		}

		if (!bKeepSurface)
		{
			//Get rid of old loaded surface
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}	
	}
}


void CTexture::UnloadTextures()
{
	//automatically deleted by the shared pointers
	m_mStore.clear();
}

STextureRef CTexture::GetTexture(const std::string& sName)
{
	auto it = m_mStore.find(sName);
	if (it == m_mStore.end())
	{
		return STextureRef();
	}
	return it->second;
}

STextureRef CTexture::LoadTexture(const std::string& sFilePath, const std::string& sName, bool bKeepSurface)
{
	const std::string& sKey = sName.size() == 0 ? sFilePath : sName;

	auto it = m_mStore.find(sKey);
	if (it != m_mStore.end())
	{
		return it->second;
	}

	auto pTex = std::make_shared<CTexture>();
	pTex->m_sFilePath = sFilePath;
	pTex->m_sName = sKey;
	pTex->m_hash = std::hash<std::string>{}(sKey);
	m_mStore.insert(std::make_pair(pTex->m_sName, pTex));
	pTex->Load(bKeepSurface);
	return pTex;
}


STextureRef CTexture::AddSurface(SDL_Surface* pSurface, const std::string& sName)
{
	std::shared_ptr<CTexture> pTex;

	auto it = m_mStore.find(sName);
	if (it != m_mStore.end())
	{
		pTex = it->second;
		if (pTex->m_pTexture)
		{
			SDL_DestroyTexture(pTex->m_pTexture);
		}
	}
	else
	{
		pTex = std::make_shared<CTexture>();
		pTex->m_sName = sName;
		pTex->m_hash = std::hash<std::string>{}(sName);
		m_mStore.insert(std::make_pair(sName, pTex));
	}

	pTex->m_pTexture = SDL_CreateTextureFromSurface(SDLManager::Instance.GetRenderer(), pSurface);
	if (pTex->m_pTexture == nullptr)
	{
		printf("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
		pTex->m_bError = true;
		pTex->m_sError = SDL_GetError();
	}
	else
	{
		
		pTex->m_Size.x = pSurface->w;
		pTex->m_Size.y = pSurface->h;
		pTex->m_bLoaded = true;
	}

	return pTex;
}


bool CTexture::DelTexture(const std::string& sName)
{
	auto it = m_mStore.find(sName);
	if (it == m_mStore.end())
	{
		return false;
	}
	m_mStore.erase(it);
	return true;
}