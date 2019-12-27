#pragma once
#include "Sprite.h"
#include "TiledMapRenderer.h"

//an asset is a description of an object that can be placed on the map
class CAsset
{
public:
	const std::string& GetName() const { return m_sName; }
	int GetBuildPrice() const { return m_nBuildPrice; }
	int GetDestroyPrice() const { return m_nDestroyPrice; }
	STileSpriteData::ETileSpriteSlot GetSpriteSlot() const { return m_eSpriteSlot; }
	const SSpriteData& GetSpriteData() const { return m_SpriteData; }

	bool LoadFromFile(const std::string& sPath);
private:
	std::string m_sName;
	int m_nBuildPrice = 0, m_nDestroyPrice = 0;

	SSpriteData m_SpriteData;
	STileSpriteData::ETileSpriteSlot m_eSpriteSlot = STileSpriteData::Object;
};

class CAssetManager
{
public:
	void LoadAssetFile(const std::string& sPath);
	CAsset* GetAsset(const std::string& sName);
private:
	std::map<std::string, CAsset> m_mAssets;
};