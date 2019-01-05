#include "GameObject.h"
#include "SDLManager.h"
#include "CollisionManager.h"

CGameObject::~CGameObject()
{
	if (bHasCollider)
	{
		CCollisionManager::RemoveCollidersForGameObject(this);
	}
}

void CGameObject::UpdateInternal(const Vec& vPos)
{
	Update();

	Vec origo = m_vPosition + vPos;

	for (unsigned int i = 0; i < m_aChildrenSprites.size(); ++i)
	{
		m_aChildrenSprites[i].first->SetPos(origo + m_aChildrenSprites[i].second);
	}

	for (unsigned int i = 0; i < m_aChildren.size(); ++i)
	{
		m_aChildren[i].first->UpdateInternal(m_aChildren[i].second + m_vPosition + vPos);
	}
}

void CGameObject::AttachSprite(std::shared_ptr<CSprite> pSprite, Vec vPos, const std::vector<std::string>& aTags)
{
	m_aChildrenSprites.push_back({ pSprite, vPos });

	for (CRenderLayer* layer : SDLManager::Instance.GetRenderLayers())
	{
		for (auto&& tag : aTags)
		{
			if (layer->HasTag(tag))
			{
				CSpriteRenderLayer* pSpritelayer = dynamic_cast<CSpriteRenderLayer*>(layer);
				if (pSpritelayer)
				{
					AddToLayer(pSpritelayer);
				}
			}
		}
	}
}

void CGameObject::AttachGameObject(std::shared_ptr<CGameObject> pGameObject, Vec vPos)
{
	m_aChildren.push_back({ pGameObject, vPos });
	pGameObject->m_pParent = this;
	
}

void CGameObject::AddSocket(const std::string & sName, Vec vPos)
{
	m_mSockets.insert({ sName, vPos });
}

Vec CGameObject::GetSocketPos(const std::string & sName)
{
	auto it = m_mSockets.find(sName);
	if (it == m_mSockets.end())
		return m_vPosition;

	return it->second + m_vPosition;
}

void CGameObject::AddToLayer(CSpriteRenderLayer * pLayer)
{
	for (unsigned int i = 0; i < m_aChildrenSprites.size(); ++i)
	{
		pLayer->AddSprite(m_aChildrenSprites[i].first);
	}

	for (unsigned int i = 0; i < m_aChildren.size(); ++i)
	{
		m_aChildren[i].first->AddToLayer(pLayer);
	}
}

void CGameObject::DetachParent()
{
	if (!m_pParent) return;

	CGameObject* pRoot = this;
	while (pRoot->m_pParent)
	{
		pRoot = pRoot->m_pParent;
	}

	std::shared_ptr<CGameObject> pDis;
	Vec vPos = GetAbsolutePos() - m_vPosition;

	for (auto it = m_pParent->m_aChildren.begin(); it != m_pParent->m_aChildren.end(); ++it)
	{
		if (it->first.get() == this)
		{
			pDis = it->first;
			m_pParent->m_aChildren.erase(it);
			break;
		}
	}	

	pRoot->AttachGameObject(pDis, vPos);
}

Vec CGameObject::GetAbsolutePos()
{
	Vec pos = m_vPosition;
	if (m_pParent)
	{
		for (auto it = m_pParent->m_aChildren.begin(); it != m_pParent->m_aChildren.end(); ++it)
		{
			if (it->first.get() == this)
			{
				pos += it->second;
				break;
			}
		}

		pos += m_pParent->GetAbsolutePos();
	}

	return pos;
}

void CGameObject::Destroy()
{
	m_aChildrenSprites.clear();
	for (unsigned int i = 0; i < m_aChildren.size(); ++i)
	{
		m_aChildren[i].first->Destroy();
	}
	m_aChildren.clear();
}

void CGameObject::DestroyChild(CGameObject* pChild)
{
	for (unsigned int i = 0; i < m_aChildren.size(); ++i)
	{
		if (m_aChildren[i].first.get() == pChild)
		{
			pChild->Destroy();
			m_aChildren.erase(m_aChildren.begin() + i);
		}
	}
}

void CGameObject::DestroySelf()
{
	if (m_pParent)
	{
		m_pParent->DestroyChild(this);
	}
}

void CGameObject::AddCollider(const IntRect& rect, bool bDynamic, bool bListen)
{
	CollisionComponent Comp;
	Comp.bListener = bListen;
	Comp.pGameObject = this;
	Comp.rect = rect;
	if (bDynamic)
	{
		CCollisionManager::AddDynamicCollider(Comp);
	}
	else
	{
		CCollisionManager::AddStaticCollider(Comp);
	}
	bHasCollider = true;
}