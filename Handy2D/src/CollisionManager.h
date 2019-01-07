#pragma once
#include "GameObject.h"

#define COLLIDE 1
#define COLLIDE_EVENTS 2
#define OVERLAP 4
#define OVERLAP_EVENTS 4

struct CollisionComponent
{
	std::weak_ptr<CGameObject> pGameObject;
	IntRect rect;
	int flags;

	CollisionComponent() {};
	CollisionComponent(std::weak_ptr<CGameObject> pGameObject, const IntRect& rect, int flags)
	{
		this->pGameObject = pGameObject;
		this->rect = rect;
		this->flags = flags;
	}
};

struct SOverlapEvent
{
	std::shared_ptr<CGameObject> pOther;
};

struct SCollisionEvent
{
	std::shared_ptr<CGameObject> pOther;
};


struct SColliderMovement
{
	IntRect From;
	IntRect To;
	IntVec vMove;
};


struct SColliderMoveInstruct
{
	CGameObject* pObject;
	IntVec vMove;
};


class CCollisionManager
{
	static std::vector<CollisionComponent> m_aStaticColliders;
	static std::vector<CollisionComponent> m_aDynamicColliders;
	static std::vector<SColliderMovement> m_aDynamicCurrent;
	static std::vector<SColliderMoveInstruct> m_aMoves;
public:
	static void Collide();
	static void AddStaticCollider(const CollisionComponent& Collider);
	static void AddDynamicCollider(const CollisionComponent& Collider);
	static void AddInstruct(const SColliderMoveInstruct& Move);
};
