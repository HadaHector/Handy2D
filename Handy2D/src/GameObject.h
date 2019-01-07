#pragma once
#include <vector>
#include "Geometry.h"
#include "Sprite.h"

struct SOverlapEvent;
struct SCollisionEvent;

class CGameObject : public std::enable_shared_from_this<CGameObject>
{
private:
	std::vector<std::pair<std::shared_ptr<CSprite>, Vec>> m_aChildrenSprites;
	std::vector<std::pair<std::shared_ptr<CGameObject>, Vec>> m_aChildren;
	std::map<std::string, Vec> m_mSockets;
	CGameObject* m_pParent = nullptr;
	void Destroy();
	
	bool bHasCollider = false;
	Vec m_vMove;
	Vec m_vPosition;

	bool m_bInited = false;
	
public:
	CGameObject() {};
	virtual ~CGameObject();

	void UpdateInternal(const Vec& vPos);

	void AttachSprite(std::shared_ptr<CSprite> pSprite, Vec vPos, const std::vector<std::string>& aTags);
	void AttachGameObject(std::shared_ptr<CGameObject> pGameObject, Vec vPos);
	void AddSocket(const std::string& sName, Vec vPos);
	Vec GetSocketPos(const std::string& sName);
	void AddToLayer(CSpriteRenderLayer* pLayer);
	void DetachParent();
	CGameObject* GetParent() { return m_pParent;}
	Vec GetAbsolutePos();
	Vec GetPos() { return m_vPosition; }
	void Move(const Vec& vec) { m_vPosition += vec; }
	void AddMovement(const Vec& vec);
	void DestroyChild(CGameObject* pChild);
	void DestroySelf();
	void SetPosition(const Vec& pos) { m_vPosition = pos; }
	void AddCollider(const IntRect& rect, bool bDynamic, int flags);


	virtual void OnOverlap(const SOverlapEvent& event) {}
	virtual void OnCollide(const SCollisionEvent& event) {}
	virtual void Update() {};
	virtual void Init() {};
};

