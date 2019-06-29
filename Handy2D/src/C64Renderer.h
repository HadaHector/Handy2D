#pragma once
#include "RenderLayer.h"
#include "Color.h"
#include "PaintedImage.h"

enum class EC64Color
{
	black,		//0
	white,		//1
	red,		//2
	cyan,		//3
	purple,		//4
	green,		//5
	blue,		//6
	yellow,		//7
	orange,		//8
	brown,		//9
	pink,		//10
	darkgrey,	//11
	grey,		//12
	lightgreen,	//13
	lightblue,	//14
	lightgrey	//15
};

struct SC64Char
{
	EC64Color cBG = EC64Color::black, cFG = EC64Color::yellow;
	bool aPixels[8 * 8];
	bool bDirty = true;

	SC64Char()
	{
		for (int i = 0; i < 8 * 8; ++i) aPixels[i] = false;
	}
	void SetCharacter(char iChar, bool bShifted);
};

class CC64RenderLayer : public CRenderLayer
{
public:
	CC64RenderLayer(const IntRect& rect);
	virtual ~CC64RenderLayer() {};

	virtual void Render() override;
	virtual void HandleEvents() override {};
	void SetName(const std::string& sName);

	SC64Char GetCharacter(int x, int y) const;
	void SetCharacter(int x, int y, const SC64Char& Char);
	void SetPixel(int x, int y, bool bOn);

	void Redraw();

protected:
	SC64Char m_aChars[40 * 25];

	CPaintedImage m_Painted;
	bool m_bDirty = true;
	std::string m_sName = "C64Renderer";
	std::shared_ptr<CImageSprite> m_pSprite;

	void DrawCharacter(int x, int y);
};
