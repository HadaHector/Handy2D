#pragma once
#include "SDLManager.h"
#include "RenderLayer.h"
#include <array>
#include "Input.h"
#include <algorithm>
#include "GameObject.h"
#include "GuiLayer.h"
#include <stdlib.h>
#include "CollisionManager.h"


#define MAP_SIZE_X 300
#define MAP_SIZE_Y 200
#define CIKK_CAKK_AMP 6
#define CIKK_CAKK_FREQ 0.12
#define CIKK_CAKK2_AMP 2
#define CIKK_CAKK2_FREQ 0.8
#define BASE_SIZE_X 18
#define BASE_SIZE_Y 17
#define TILE_SIZE 16

enum ETile
{
	ET_Empty,
	ET_Dirt1,
	ET_Dirt2,
	ET_Dirt3,
	ET_Dirt4,
	ET_Rock
};

class CImpact : public CGameObject
{
	float m_fLifetime = 0.0f;
public:
	virtual void Init();
	virtual void Update();
};


class CBase : public CGameObject
{
	int m_nPlayer;
	std::shared_ptr<CImageSprite> m_pBase;
public:
	void SetPlayer(int nPlayer);
	virtual void Init();
};

class CRechargeArea : public CGameObject
{
public:
	virtual void Init();
	int m_nPlayer;
};

class CProjectile : public CGameObject
{
	IntVec m_vSpeed;
	int m_nPlayer;
public:
	virtual void Init();
	virtual void Update();
	void SetSpeed(const IntVec& spd);
	void SetPlayer(int nPlayer);
	void Explode(const Vec& pos);
	bool CollideTerrain(const Vec& pos);
	virtual void OnMove(const Vec& from, const Vec& to) override;
	virtual void OnOverlap(const SOverlapEvent& event) override;
};


class CRat : public CGameObject
{
private:
	int m_nStep = 0;
	double m_timer = 0;
	double m_cooldown = 0;
	double m_freezetimer = 0;
	std::shared_ptr<CAnimSprite> m_pRat;

	const float m_fMaxHealth = 100;
	const float m_fMaxEnergy = 100;
	float m_fHealth = m_fMaxHealth;
	float m_fEnergy = m_fMaxEnergy;
	Vec m_vDir;

	bool m_bDead = false;
public:

	float GetHealthRate() { return m_fHealth / m_fMaxHealth; }
	float GetEnergyRate() { return m_fEnergy / m_fMaxEnergy; }
	virtual void Init();
	void Fire();
	virtual void Update();
	void Die();
	void Damage(int nDmg);
	void Freeze(float time);

	virtual void OnMove(const Vec& from, const Vec& to) override;
	virtual void OnOverlap(const SOverlapEvent& Event) override;
	int m_nPlayer = 0;

};

class CRatGame;

struct CDirtLayer
{
	std::vector<std::shared_ptr<CImageSprite>> m_aDirtSprites;
	CSpriteRenderLayer *pDirtLayer;

	void Load();
	void Update(IntVec vPos);
	void OnResize();

	std::weak_ptr<CTexture> pDirt1, pDirt2, pDirt3, pDirt4, pRock;

};

class CRatGame : public CGameLogic
{
	std::vector<CGuiImage*> aImages;
	std::shared_ptr<CGameObject> root;
	std::weak_ptr<CRat> m_rat1, m_rat2;
	std::weak_ptr<CBase> m_base1, m_base2;
	std::weak_ptr<CRechargeArea> m_recharge1, m_recharge2;
	std::vector<std::shared_ptr<CImageSprite>> m_aBgs;
	CCameraRenderLayer *pCamera1, *pCamera2;
	CCameraRenderLayer *pBgCamera1, *pBgCamera2;
	CDirtLayer m_DirtLayer1, m_DirtLayer2;
	CGuiLayer *pGui;

	CGuiImage *pLeftBar1Bg, *pLeftBar2Bg, *pRightBar1Bg, *pRightBar2Bg;
	CGuiImage *pLeftHpBar, *pLeftEnergyBar, *pRightHpBar, *pRightEnergyBar;
	CGuiImage *pLeftEnergyIcon, *pLeftHealthIcon, *pRightEnergyIcon, *pRightHealthIcon;
	CGuiImage *pMenuBg, *pTitleImg, *pStartButton, *pOverviewMapImage, *pGameOverBg;
	CGuiTextbox *pPlayerText1, *pPlayerText2, *pStartText, *pScore, *pGameOverText;

	CGuiText *pStart, *pFPS;
	CGuiImage* pBg[5];

	class CPaintedImage *pMapImage;

	ETile mapdata[MAP_SIZE_Y][MAP_SIZE_X];

	float fHp1, fHp2, fEnergy1, fEnergy2;
	bool m_bResized = false;
	static CRatGame* m_pInstance;

	IntVec m_vSpawnPos1, m_vSpawnPos2;

	int scores[2] = { 0,0 };
	int nRound = 0;

	double roundrestarttimer = -1;
	IntVec vExplosion;
	double explosionTimer = -1;

	CGuiElement* pMenu;

public:
	CRatGame() { m_pInstance = this; }
	ETile GetTile(IntVec pos);
	ETile GetTileAtPos(Vec pos);
	void BreakTile(IntVec pos);
	void BreakTileAtPos(Vec pos);
	void SetTile(IntVec pos, ETile tile);
	void SetTileAtPos(Vec pos, ETile tile);
	virtual bool Load() override;
	void Resize();
	void UpdateGui();
	virtual void Update() override;
	virtual void OnResize() override;
	static CRatGame* GetInstance() { return m_pInstance; }
	void AddScore(int nPlayer);
	void UpdateScore();
	void SpawnRats();
	void StartExplosion(IntVec pos);
	void Explode(float strength, int count);
	void ResetGame();
	void GameOver();
};

