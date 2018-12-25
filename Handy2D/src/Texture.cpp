#include "Texture.h"
#include <SDL.h>
#include <SDL_image.h>
#include "SDLManager.h"


std::map<std::string, std::shared_ptr<CTexture>> CTexture::m_mStore;

CTexture::CTexture()
{

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

std::weak_ptr<CTexture> CTexture::GetTexture(std::string sName)
{
	auto it = m_mStore.find(sName);
	if (it == m_mStore.end())
	{
		return std::weak_ptr<CTexture>();
	}
	return it->second;
}

std::weak_ptr<CTexture> CTexture::LoadTexture(std::string sFilePath, std::string sName)
{
	auto pTex = std::make_shared<CTexture>();
	pTex->m_sFilePath = sFilePath;
	pTex->m_sName = sName.size() == 0 ? sFilePath : sName;
	m_mStore.insert(std::make_pair(pTex->m_sName, pTex)).first->second;
	pTex->Load();
	return pTex;
}
