#pragma once
#include <vector>
#include "Geometry.h"
#include "Sprite.h"

class CGameObject
{
private:
	std::vector<std::pair<std::shared_ptr<CSprite>, Vec>> m_aChildrenSprites;
	std::vector<std::pair<std::shared_ptr<CGameObject>, Vec>> m_aChildren;
	std::map<std::string, Vec> m_mSockets;
	CGameObject* m_pParent = nullptr;
	void Destroy();
protected:
	Vec m_vPosition;
public:
	CGameObject() {};
	virtual ~CGameObject() {};

	void UpdateInternal(const Vec& vPos);

	void AttachSprite(std::shared_ptr<CSprite> pSprite, Vec vPos);
	void AttachGameObject(std::shared_ptr<CGameObject> pGameObject, Vec vPos);
	void AddSocket(const std::string& sName, Vec vPos);
	Vec GetSocketPos(const std::string& sName);
	void AddToLayer(CSpriteRenderLayer* pLayer);
	void DetachParent();
	Vec GetAbsolutePos();
	void DestroyChild(CGameObject* pChild);
	void DestroySelf();
	void SetPosition(const Vec& pos) { m_vPosition = pos; }

	virtual void Update() {};
};

