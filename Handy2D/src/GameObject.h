#pragma once
#include <vector>
#include "Geometry.h"
#include "Sprite.h"

struct SOverlapEvent;

class CGameObject
{
private:
	std::vector<std::pair<std::shared_ptr<CSprite>, Vec>> m_aChildrenSprites;
	std::vector<std::pair<std::shared_ptr<CGameObject>, Vec>> m_aChildren;
	std::map<std::string, Vec> m_mSockets;
	std::vector<IntRect> m_aCollideBoxes;
	CGameObject* m_pParent = nullptr;
	void Destroy();
	
	bool bHasCollider = false;

protected:
	Vec m_vPosition;
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
	void DestroyChild(CGameObject* pChild);
	void DestroySelf();
	void SetPosition(const Vec& pos) { m_vPosition = pos; }
	void AddCollider(const IntRect& rect, bool bDynamic, bool bListen);


	virtual void OnCollide(const SOverlapEvent& event) {}
	virtual void Update() {};
};

