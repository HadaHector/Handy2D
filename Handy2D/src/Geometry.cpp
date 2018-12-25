#include "Geometry.h"
#include <SDL.h>


Vec::operator SDL_Point() const
{
	SDL_Point P;
	P.x = (int)roundf(x);
	P.y = (int)roundf(y);
	return P;
}

IntVec::operator SDL_Point() const
{
	SDL_Point P;
	P.x = x;
	P.y = y;
	return P;
}

IntRect::operator SDL_Rect() const
{
	SDL_Rect Rect;
	Rect.x = m_vPos.x;
	Rect.y = m_vPos.y;
	Rect.w = m_vSize.x;
	Rect.h = m_vSize.y;
	return Rect;
}

IntRect::IntRect(const SDL_Rect& rect)
{
	m_vPos.x = rect.x;
	m_vPos.y = rect.y;
	m_vSize.x = rect.w;
	m_vSize.y = rect.h;
}