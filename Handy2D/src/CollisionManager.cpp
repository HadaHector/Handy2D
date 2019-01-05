#include "CollisionManager.h"

std::vector<CollisionComponent> CCollisionManager::m_aStaticColliders;
std::vector<CollisionComponent> CCollisionManager::m_aDynamicColliders;
std::vector<IntRect> CCollisionManager::m_aDynamicCurrent;
std::vector<const CGameObject*> CCollisionManager::m_aPendingDeletes;

void CCollisionManager::Collide()
{
	//removing pending deletes
	for (int i = m_aDynamicColliders.size() - 1; i > 0; i--)
	{
		for (const CGameObject* pObj : m_aPendingDeletes)
		{
			if (m_aDynamicColliders[i].pGameObject == pObj)
			{
				m_aDynamicColliders.erase(m_aDynamicColliders.begin() + i);
			}
		}
	}

	for (int i = m_aStaticColliders.size() - 1; i > 0; i--)
	{
		for (const CGameObject* pObj : m_aPendingDeletes)
		{
			if (m_aStaticColliders[i].pGameObject == pObj)
			{
				m_aStaticColliders.erase(m_aStaticColliders.begin() + i);
			}
		}
	}

	m_aPendingDeletes.clear();


	//updating dynamic colliders
	int nDynamicCount = m_aDynamicColliders.size();
	m_aDynamicCurrent.resize(nDynamicCount);

	for (int i=0; i< nDynamicCount; ++i)
	{
		m_aDynamicCurrent[i].SetPos(m_aDynamicColliders[i].rect.GetUpperLeft() + m_aDynamicColliders[i].pGameObject->GetAbsolutePos());
		m_aDynamicCurrent[i].SetSize(m_aDynamicColliders[i].rect.GetSize());
	}

	//dynamic - non dyamic collision
	for (int i = 0; i < nDynamicCount; ++i)
	{
		for (auto&& Static : m_aStaticColliders)
		{
			if (Static.rect.HasIntersection(m_aDynamicCurrent[i]))
			{
				if (m_aDynamicColliders[i].bListener)
				{
					SOverlapEvent e;
					e.pOther = Static.pGameObject;
					m_aDynamicColliders[i].pGameObject->OnCollide(e);
				}
				if (Static.bListener)
				{
					SOverlapEvent e;
					e.pOther = m_aDynamicColliders[i].pGameObject;
					Static.pGameObject->OnCollide(e);
				}
			}
		}
	}

	//dynamic - dyamic collision
	for (int i = 1; i < nDynamicCount; ++i)
	{
		for (int j = 0; j < i; ++j)
		{
			if (m_aDynamicCurrent[i].HasIntersection(m_aDynamicCurrent[j]))
			{
				if (m_aDynamicColliders[i].bListener)
				{
					SOverlapEvent e;
					e.pOther = m_aDynamicColliders[j].pGameObject;
					m_aDynamicColliders[i].pGameObject->OnCollide(e);
				}
				if (m_aDynamicColliders[j].bListener)
				{
					SOverlapEvent e;
					e.pOther = m_aDynamicColliders[i].pGameObject;
					m_aDynamicColliders[j].pGameObject->OnCollide(e);
				}
			}
		}
	}
}

void CCollisionManager::AddStaticCollider(const CollisionComponent& Collider)
{
	m_aStaticColliders.push_back(Collider);
}


void CCollisionManager::AddDynamicCollider(const CollisionComponent& Collider)
{
	m_aDynamicColliders.push_back(Collider);
}


void CCollisionManager::RemoveCollidersForGameObject(const CGameObject* pObj)
{
	m_aPendingDeletes.push_back(pObj);
}