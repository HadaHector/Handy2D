#include "Geometry.h"
#include <SDL.h>


Vec::operator SDL_Point() const
{
	SDL_Point P;
	P.x = (int)roundf(x);
	P.y = (int)roundf(y);
	return P;
}

Vec::operator IntVec() const
{
	return IntVec((int)roundf(x), (int)roundf(y));
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

bool IntRect::HasIntersection(const IntRect& other) const
{
	int Amin = m_vPos.x;
	int Amax = m_vPos.x + m_vSize.x;
	int Bmin = other.m_vPos.x;
	int Bmax = other.m_vPos.x + other.m_vSize.x;
	if (Bmin > Amin)
		Amin = Bmin;
	if (Bmax < Amax)
		Amax = Bmax;
	if (Amax <= Amin)
		return false;

	/* Vertical intersection */
	Amin = m_vPos.y;
	Amax = m_vPos.y + m_vSize.y;
	Bmin = other.m_vPos.y;
	Bmax = other.m_vPos.y + other.m_vSize.y;
	if (Bmin > Amin)
		Amin = Bmin;
	if (Bmax < Amax)
		Amax = Bmax;
	if (Amax <= Amin)
		return false;

	return true;
}

bool IntRect::IsInside(IntVec vPos) const
{
	IntVec vTrans = vPos - m_vPos;
	return (vTrans.x >= 0 && vTrans.y >= 0 && vTrans.x <= m_vSize.x && vTrans.y <= m_vSize.y);
}

void IntRect::Expand(int nAmount)
{
	m_vPos.x -= nAmount;
	m_vPos.y -= nAmount;
	m_vSize.x += nAmount * 2;
	m_vSize.y += nAmount * 2;
}