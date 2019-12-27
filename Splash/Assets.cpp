#include "Assets.h"
#include "json11.hpp"
#include <fstream>
#include <streambuf>

bool CAsset::LoadFromFile(const std::string & sPath)
{
	using namespace json11;
	std::ifstream file(sPath.c_str());
	std::string str((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());

	std::string sError;
	Json jData = Json::parse(str.c_str(), sError);
	if (sError.size() > 0)
	{
		return false;
	}

	m_sName = jData["name"].string_value();
	m_nBuildPrice = jData["buildprice"].int_value();
	m_nDestroyPrice = jData["destroyprice"].int_value();

	m_SpriteData.m_sData = jData["spritedata"].string_value();
	m_SpriteData.m_nHeight = jData["spriteheight"].int_value();

	std::string sTmp = jData["spriteslot"].string_value();
	if (sTmp == "" || sTmp == "object")
	{
		m_eSpriteSlot = STileSpriteData::Object;
	}
	else if (sTmp == "fence")
	{
		m_eSpriteSlot = STileSpriteData::Side0;
	}
	else if (sTmp == "smallobject")
	{
		m_eSpriteSlot = STileSpriteData::SmallObject0;
	}
	else if (sTmp == "ground")
	{
		m_eSpriteSlot = STileSpriteData::Ground;
	}

	if (jData["sprites"].is_array())
	{
		for (const Json& item : jData["sprites"].array_items())
		{
			std::string sTex = item.string_value();
			STextureRef Tex = CTexture::LoadTexture(sTex);
			
			std::shared_ptr<CImageSprite> pSprite = std::make_shared<CImageSprite>();
			pSprite->SetTexture(Tex);
			if (Tex)
			{
				pSprite->SetSize(Tex.lock()->GetSize());
			}
			m_SpriteData.m_aSprites.push_back(pSprite);
		}
	}

	return true;
}

void CAssetManager::LoadAssetFile(const std::string & sPath)
{
	CAsset Asset;
	if (Asset.LoadFromFile(sPath))
	{
		m_mAssets.insert(std::make_pair(Asset.GetName(), Asset));
	}
}

CAsset * CAssetManager::GetAsset(const std::string & sName)
{
	auto it = m_mAssets.find(sName);
	if (it != m_mAssets.end())
	{
		return &it->second;
	}
	return nullptr;
}
