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

void CTexture::Load()
{
	SDL_Surface* loadedSurface = IMG_Load(m_sFilePath.c_str());
	if (loadedSurface == nullptr)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", m_sFilePath.c_str(), IMG_GetError());
		m_bError = true;
		m_sError = IMG_GetError();
	}
	else
	{
		//Create texture from surface pixels
		m_pTexture = SDL_CreateTextureFromSurface(SDLManager::Instance.GetRenderer(), loadedSurface);
		if (m_pTexture == nullptr)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", m_sFilePath.c_str(), SDL_GetError());
			m_bError = true;
			m_sError = SDL_GetError();
		}
		else
		{
			m_Size.x = loadedSurface->w;
			m_Size.y = loadedSurface->h;			
			m_bLoaded = true;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
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

STextureRef CTexture::LoadTexture(const std::string& sFilePath, const std::string& sName)
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
	pTex->Load();
	return pTex;
}


STextureRef CTexture::AddSurface(SDL_Surface* pSurface, const std::string& sName)
{
	auto pTex = std::make_shared<CTexture>();
	pTex->m_sName = sName;
	pTex->m_hash = std::hash<std::string>{}(sName);
	m_mStore.insert(std::make_pair(sName, pTex));


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