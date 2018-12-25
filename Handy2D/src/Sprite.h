#pragma once
#include <memory>
#include "Texture.h"
#include "Geometry.h"
#include "RenderLayer.h"
#include "Color.h"

class CRenderLayer;

class CSprite
{
public:
	CSprite() {}
	virtual ~CSprite() {}
	virtual void Render(const CRenderLayer& Layer) {}


	Vec GetPos() { return m_Position; }
	void SetPos(Vec Pos) { m_Position = Pos; }
	void SetPos(float x, float y) { m_Position = { x,y }; }
	void SetPosX(float x) { m_Position.x = x; }
	void SetPosY(float y) { m_Position.y = y; }
	void Move(Vec Delta) { m_Position += Delta; }

	Vec GetSize() { return m_Size; }
	void SetSize(Vec Pos) { m_Size = Pos; }
	void SetSize(float x, float y) { m_Size = { x,y }; }
	void SetSizeX(float x) { m_Size.x = x; }
	void SetSizeY(float y) { m_Size.y = y; }

	SDL_Rect GetRect();

protected:
	Vec m_Position, m_Size;
};


class CImageSprite : public CSprite
{
public:
	CImageSprite();
	virtual ~CImageSprite();
	virtual void Render(const CRenderLayer& Layer);

	virtual void SetTexture(std::weak_ptr<CTexture> pTexture);

protected:
	std::weak_ptr<CTexture> m_pTexture;
};

class CAnimSprite : public CImageSprite
{
public:
	virtual ~CAnimSprite();

	virtual void Render(const CRenderLayer& Layer);
	void SetTexture(std::weak_ptr<CTexture> pTexture);
	void SetRowsAndCols(int row, int col)
	{
		m_nRows = row;
		m_nCols = col;
		RefreshTileSizes();
	}
	void SetFrameTime(double time)
	{
		m_frametime = time;
	}
	void SetFrame(int n)
	{
		m_nFrame = n;
	}
private:
	void RefreshTileSizes();

	int m_nRows = 1, m_nCols = 1;
	Vec m_TileSize, m_Size;
	double m_clock = 0, m_frametime = 0;
	int m_nFrame = 0;
};

struct _TTF_Font;

class CTextSprite : public CSprite
{
public:
	virtual ~CTextSprite();

	virtual void Render(const CRenderLayer& Layer);
	virtual void SetTexture(std::weak_ptr<CTexture> pTexture) {}; //not used

	void SetText(std::string sText) { m_sText = sText; }
	std::string GetText() { return m_sText; }
	void SetColor(Color sColor) { m_Color = sColor; }
	Color GetColor() { return m_Color; }

private:
	std::string m_sText, m_sLastText;
	Vec m_TextSize;
	bool m_bCreateError = false;
	Color m_Color = { 0,0,0,0 };
	SDL_Texture* m_pTextTexture = nullptr;
	_TTF_Font* m_pFont = nullptr;
};
