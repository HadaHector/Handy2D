#pragma once
#include "GameObject.h"

struct CollisionComponent
{
	CGameObject* pGameObject;
	IntRect rect;
	bool bListener;

	CollisionComponent() {};
	CollisionComponent(CGameObject* pGameObject, const IntRect& rect, bool bListener)
	{
		this->pGameObject = pGameObject;
		this->rect = rect;
		this->bListener = bListener;
	}
};

struct SOverlapEvent
{
	CGameObject* pOther;
};


class CCollisionManager
{
	static std::vector<CollisionComponent> m_aStaticColliders;
	static std::vector<CollisionComponent> m_aDynamicColliders;
	static std::vector<IntRect> m_aDynamicCurrent;
	static std::vector<const CGameObject*> m_aPendingDeletes;
public:
	static void Collide();
	static void AddStaticCollider(const CollisionComponent& Collider);
	static void AddDynamicCollider(const CollisionComponent& Collider);
	static void RemoveCollidersForGameObject(const CGameObject* pObj);
};
