#pragma once

struct SDL_Color;

struct Color
{
	unsigned char r = 0, g = 0, b = 0, a = 0;
	Color() {};
	Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) :
		r(_r), g(_g), b(_b), a(_a) {};

	operator SDL_Color() const;

};