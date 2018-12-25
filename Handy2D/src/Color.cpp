#include "Color.h"
#include "SDL.h"

Color::operator SDL_Color() const
{
	SDL_Color c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}