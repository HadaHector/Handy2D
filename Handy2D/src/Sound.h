#pragma once
#include <string>
#include <map>
#include <memory>


class CSound
{
	struct Mix_Chunk* m_pChunk = nullptr;
	float m_fLength;

	std::string m_sFilePath;
	std::string m_sName;
	bool m_bLoaded = false;
	bool m_bError = false;
	std::string m_sError;

	void Load();
public:
	CSound();
	virtual ~CSound();
	static std::weak_ptr<CSound> LoadSound(const std::string& sFilePath, const std::string& sName = "");
	static std::weak_ptr<CSound> GetSound(const std::string& sName);
	static std::map<std::string, std::shared_ptr<CSound>> m_mStore;
	static bool DelSound(const std::string& sName);
	Mix_Chunk* GetChunk() { return m_pChunk; }
};