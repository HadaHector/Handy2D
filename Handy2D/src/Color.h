#pragma once

struct SDL_Color;
struct SDL_Surface;

struct Color
{
	unsigned char r = 0, g = 0, b = 0, a = 0;
	Color() {};
	Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) :
		r(_r), g(_g), b(_b), a(_a) {}
	Color(unsigned int code)
	{
		r = ((unsigned char*)(&code))[3];
		g = ((unsigned char*)(&code))[2];
		b = ((unsigned char*)(&code))[1];
		a = ((unsigned char*)(&code))[0];
	}

	operator unsigned int() const
	{
		int ret;
		((unsigned char*)(&ret))[3] = a;
		((unsigned char*)(&ret))[2] = b;
		((unsigned char*)(&ret))[1] = g;
		((unsigned char*)(&ret))[0] = r;
		return ret;
	}

	unsigned int forSurface(SDL_Surface* pSurface);

	operator SDL_Color() const;
	
};
