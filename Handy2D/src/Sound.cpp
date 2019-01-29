#include "SDL_mixer.h"
#include "Sound.h"

std::map<std::string, std::shared_ptr<CSound>> CSound::m_mStore;

CSound::CSound()
{

}

CSound::~CSound()
{
	if (m_pChunk)
	{
		Mix_FreeChunk(m_pChunk);
	}
}

std::weak_ptr<CSound> CSound::LoadSound(const std::string& sFilePath, const std::string& sName)
{
	const std::string& sKey = sName.size() == 0 ? sFilePath : sName;

	auto it = m_mStore.find(sKey);
	if (it != m_mStore.end())
	{
		return it->second;
	}

	auto pSound = std::make_shared<CSound>();
	pSound->m_sFilePath = sFilePath;
	pSound->m_sName = sKey;
	m_mStore.insert(std::make_pair(pSound->m_sName, pSound));
	pSound->Load();
	return pSound;
}


void CSound::Load()
{
	m_pChunk = Mix_LoadWAV(m_sFilePath.c_str());
	if (m_pChunk == nullptr)
	{
		printf("Unable to load sound from %s! SDL_mixer Error: %s\n", m_sFilePath.c_str(), Mix_GetError());
		m_bError = true;
		m_sError = Mix_GetError();
	}
	else
	{
		m_fLength = (float) (m_pChunk->alen / ((44100 * 2 * 2) / 1000));
		m_bLoaded = true;
	}
}

std::weak_ptr<CSound> CSound::GetSound(const std::string& sName)
{
	auto it = m_mStore.find(sName);
	if (it == m_mStore.end())
	{
		return std::weak_ptr<CSound>();
	}
	return it->second;
}


bool CSound::DelSound(const std::string& sName)
{
	auto it = m_mStore.find(sName);
	if (it == m_mStore.end())
	{
		return false;
	}
	m_mStore.erase(it);
	return true;
}

