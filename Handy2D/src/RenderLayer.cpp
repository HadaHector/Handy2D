#include "RenderLayer.h"

bool CRenderLayer::HasTag(const std::string& sTag) const
{
	for (auto&& tag : m_aTags) 
	{ 
		if (tag == sTag) return true;
	} 
	return false;
}

void CSpriteRenderLayer::Render()
{
	if (m_bVisible)
	{
		std::vector<int> aBadIndices;

		for (unsigned int i=0; i<m_aSprites.size(); ++i)
		{
			auto pSprite = m_aSprites[i].lock();
			if (pSprite)
			{
				pSprite->Render(*this);
			}
			else
			{
				aBadIndices.push_back(i);
			}
		}

		for (int i = aBadIndices.size()-1; i>=0; --i)
		{
			m_aSprites.erase(m_aSprites.begin() + aBadIndices[i]);
		}
	}
}

void CSpriteRenderLayer::AddSprite(std::weak_ptr<CSprite> pSprite)
{
	m_aSprites.push_back(pSprite);
}

void CSpriteRenderLayer::RemoveSprite(std::weak_ptr<CSprite> pSprite)
{
	for (auto it = m_aSprites.begin(); it != m_aSprites.end(); ++it)
	{
		if (it->owner_before(pSprite))
		{
			m_aSprites.erase(it);
			return;
		}
	}
	
}

void CSpriteRenderLayer::ClearSprites()
{
	m_aSprites.clear();
}


void CCameraRenderLayer::Render()
{
	if (m_bVisible)
	{
		std::vector<int> aBadIndices;

		for (unsigned int i = 0; i < m_aSprites.size(); ++i)
		{
			auto pSprite = m_aSprites[i].lock();
			if (pSprite)
			{
				pSprite->Render(*this, -m_vPosition + m_Rect.GetUpperLeft());
			}
			else
			{
				aBadIndices.push_back(i);
			}
		}

		for (int i = aBadIndices.size() - 1; i >= 0; --i)
		{
			m_aSprites.erase(m_aSprites.begin() + aBadIndices[i]);
		}
	}
}

