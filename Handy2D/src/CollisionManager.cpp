#include "CollisionManager.h"
#include <algorithm>
#include <vector>


std::vector<CollisionComponent> CCollisionManager::m_aStaticColliders;
std::vector<CollisionComponent> CCollisionManager::m_aDynamicColliders;
std::vector<SColliderMovement> CCollisionManager::m_aDynamicCurrent;
std::vector<SColliderMoveInstruct> CCollisionManager::m_aMoves;


IntVec LimitMove(const IntVec& vMove, const IntRect& From, const IntRect& Against)
{
	IntVec vNewMove;

	if (vMove.x > 0)
	{
		int b1 = From.GetUpperLeft().x + From.GetSize().x;
		int a2 = Against.GetUpperLeft().x;
		vNewMove.x = std::min(vMove.x, a2 - b1);
	}
	else if (vMove.x < 0)
	{
		int a1 = From.GetUpperLeft().x;
		int b2 = Against.GetUpperLeft().x + Against.GetSize().x;
		vNewMove.x = std::max(vMove.x, b2 - a1);
	}

	if (vMove.y > 0)
	{
		int b1 = From.GetUpperLeft().y + From.GetSize().y;
		int a2 = Against.GetUpperLeft().y;
		vNewMove.y = std::min(vMove.y, a2 - b1);
	}
	else if (vMove.y < 0)
	{
		int a1 = From.GetUpperLeft().y;
		int b2 = Against.GetUpperLeft().y + Against.GetSize().y;
		vNewMove.y = std::max(vMove.y, b2 - a1);
	}

	return vNewMove;
}

void CCollisionManager::Collide()
{
	//removing unreferenced gameobjects
	for (int i = m_aDynamicColliders.size() - 1; i > 0; i--)
	{
		if (m_aDynamicColliders[i].pGameObject.expired())
		{
			m_aDynamicColliders.erase(m_aDynamicColliders.begin() + i);
		}
	}

	for (int i = m_aStaticColliders.size() - 1; i > 0; i--)
	{
		if (m_aStaticColliders[i].pGameObject.expired())
		{
			m_aStaticColliders.erase(m_aStaticColliders.begin() + i);
		}
	}


	//updating dynamic colliders
	int nDynamicCount = m_aDynamicColliders.size();
	m_aDynamicCurrent.resize(nDynamicCount);

	int nMovedCount = m_aMoves.size();

	for (int i=0; i< nDynamicCount; ++i)
	{
		m_aDynamicCurrent[i].From.SetPos(m_aDynamicColliders[i].rect.GetUpperLeft() + m_aDynamicColliders[i].pGameObject.lock()->GetAbsolutePos());
		m_aDynamicCurrent[i].From.SetSize(m_aDynamicColliders[i].rect.GetSize());
		m_aDynamicCurrent[i].To = m_aDynamicCurrent[i].From;
		for (auto Move : m_aMoves)
		{
			if (m_aDynamicColliders[i].pGameObject.lock().get() == Move.pObject)
			{
				m_aDynamicCurrent[i].vMove = Move.vMove;
				m_aDynamicCurrent[i].To.SetPos(m_aDynamicCurrent[i].From.GetUpperLeft() + Move.vMove);
				m_aDynamicCurrent[i].To.SetSize(m_aDynamicColliders[i].rect.GetSize());
				break;
			}
		}
	}

	m_aMoves.clear();

	std::vector<std::pair< SOverlapEvent, std::shared_ptr<CGameObject>>> aOverlapEvents;
	std::vector<std::pair< SCollisionEvent, std::shared_ptr<CGameObject>>> aCollisionEvents;
	
	//collisions and overlaps
	for (int i = 0; i < nDynamicCount; ++i)
	{
		CollisionComponent& Dynamic = m_aDynamicColliders[i];
		SColliderMovement& Movement = m_aDynamicCurrent[i];

		IntVec vMovement = Movement.vMove;
		IntVec vMovementStart = vMovement;

		if (Dynamic.flags & COLLIDE || Dynamic.flags & OVERLAP)
		{
			//test with statics
			for (auto&& Static : m_aStaticColliders)
			{
				if (Dynamic.flags & COLLIDE && Static.flags & COLLIDE)
				{
					if (Static.rect.HasIntersection(Movement.To))
					{
						if (Dynamic.flags & COLLIDE_EVENTS || Static.flags & COLLIDE_EVENTS)
						{
							auto pStaticObj = Static.pGameObject.lock();
							auto pDynamicObj = Dynamic.pGameObject.lock();
							if (Dynamic.flags & COLLIDE_EVENTS)
							{
								SCollisionEvent e;
								e.pOther = pStaticObj;
								aCollisionEvents.push_back({ e, pDynamicObj });
							}
							if (Static.flags & COLLIDE_EVENTS)
							{
								SCollisionEvent e;
								e.pOther = pDynamicObj;
								aCollisionEvents.push_back({ e, pStaticObj });
							}
						}

						IntVec vMov = LimitMove(Movement.vMove, Movement.From, Static.rect);
						if (abs(vMov.x) < abs(vMovement.x)) vMovement.x = vMov.x;
						if (abs(vMov.y) < abs(vMovement.y)) vMovement.y = vMov.y;

					}
				}

				if (Dynamic.flags & OVERLAP && Static.flags & OVERLAP)
				{
					if (Static.rect.HasIntersection(Movement.To))
					{
						if (Dynamic.flags & OVERLAP_EVENTS || Static.flags & OVERLAP_EVENTS)
						{
							auto pStaticObj = Static.pGameObject.lock();
							auto pDynamicObj = Dynamic.pGameObject.lock();
							if (Dynamic.flags & OVERLAP_EVENTS)
							{
								SOverlapEvent e;
								e.pOther = pStaticObj;
								aOverlapEvents.push_back({ e, pDynamicObj });
							}
							if (Static.flags & OVERLAP_EVENTS)
							{
								SOverlapEvent e;
								e.pOther = pDynamicObj;
								aOverlapEvents.push_back({ e,pStaticObj });
							}
						}
					}
				}		
			}

			//collide with other dynamics
			if (Dynamic.flags & COLLIDE)
			{
				for (int j = 0; j < nDynamicCount; ++j)
				{
					if (i == j) continue;
					CollisionComponent& Dynamic2 = m_aDynamicColliders[j];

					if (Dynamic2.flags & COLLIDE)
					{
						if (m_aDynamicCurrent[j].From.HasIntersection(m_aDynamicCurrent[i].To))
						{
							if (Dynamic.flags & COLLIDE_EVENTS)
							{
								auto pDynamicObj = Dynamic.pGameObject.lock();
								auto pDynamic2Obj = Dynamic2.pGameObject.lock();
								SCollisionEvent e;
								e.pOther = pDynamic2Obj;
								aCollisionEvents.push_back({ e, pDynamicObj });
							}

							IntVec vMov = LimitMove(Movement.vMove, Movement.From, m_aDynamicCurrent[j].To);
							if (abs(vMov.x) < abs(vMovement.x)) vMovement.x = vMov.x;
							if (abs(vMov.y) < abs(vMovement.y)) vMovement.y = vMov.y;
						}
					}
				}
			}
			

			if (Dynamic.flags & OVERLAP)
			//test with OTHER dynamics
			for (int j = 0; j < i; ++j)
			{
				CollisionComponent& Dynamic2 = m_aDynamicColliders[j];

				if (Dynamic2.flags & OVERLAP)
				{
					if (m_aDynamicCurrent[j].From.HasIntersection(m_aDynamicCurrent[i].To))
					{
						if (Dynamic.flags & OVERLAP_EVENTS || Dynamic2.flags & OVERLAP_EVENTS)
						{
							auto pDynamicObj = Dynamic.pGameObject.lock();
							auto pDynamic2Obj = Dynamic2.pGameObject.lock();
							if (Dynamic.flags & OVERLAP_EVENTS)
							{
								SOverlapEvent e;
								e.pOther = pDynamic2Obj;
								aOverlapEvents.push_back({ e, pDynamicObj });
							}
							if (Dynamic2.flags & OVERLAP_EVENTS)
							{
								SOverlapEvent e;
								e.pOther = pDynamicObj;
								aOverlapEvents.push_back({ e, pDynamic2Obj });
							}
						}
					}
				}
			}
		}

		if (vMovement && !Dynamic.pGameObject.expired())
		{
			Dynamic.pGameObject.lock()->Move(vMovement);
		}
	}

	for (auto&& Event : aOverlapEvents)
	{
		Event.second->OnOverlap(Event.first);
	}

	for (auto&& Event : aCollisionEvents)
	{
		Event.second->OnCollide(Event.first);
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


void CCollisionManager::AddInstruct(const SColliderMoveInstruct& Move)
{
	m_aMoves.push_back(Move);
}
