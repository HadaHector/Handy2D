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

Color g_C64Colors[16] =
{
	{0,0,0,255},		//0 black
	{255,255,255,255},	//1 white
	{136,0,0,255},		//2 red
	{170,255,238,255},	//3 cyan
	{204,68,204,255},	//4 purple
	{0,204,85,255},		//5 green
	{0,0,170,255},		//6 blue
	{238,238,119,255},  //7 yellow
	{221,136,85,255},	//8 orange
	{102,68,0,255},		//9 brown
	{255,119,119,255},  //10 pink
	{51,51,51,255},     //11 dark grey
	{119,119,119,255},  //12 grey
	{170,255,102,255},  //13 light green
	{0,136,255,255},    //14 light blue
	{187,187,187,255}   //15 light grey
};

struct SC64Char
{
	EC64Color cBG, cFG;
	bool aPixels[8 * 8];
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

protected:
	SC64Char m_aChars[40 * 25];

	CPaintedImage m_Painted;
	bool m_bDirty = true;
	std::string m_sName = "C64Renderer";
	std::weak_ptr<CImageSprite> m_pSprite;

	void DrawCharacter(int x, int y);

};
