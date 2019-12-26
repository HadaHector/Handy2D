#pragma once
#include <math.h>
#include <cmath>

struct SDL_Point;
struct SDL_Rect;
class IntVec;

class Vec
{
public:
	float x, y;
	Vec() : x(0.0f), y(0.0f) {};
	Vec(float _x, float _y) : x(_x), y(_y) {};

	void operator+=(const Vec& other)
	{
		x += other.x;
		y += other.y;
	}

	Vec operator+(const Vec& other) const
	{
		return Vec(x + other.x, y + other.y);
	}

	void operator-=(const Vec& other)
	{
		x -= other.x;
		y -= other.y;
	}

	Vec operator-(const Vec& other) const
	{
		return Vec(x - other.x, y - other.y);
	}

	Vec operator-() const
	{
		return Vec(-x, -y);
	}

	template< class T >
	Vec operator/(T f) const
	{
		return Vec((float)(x / f), (float)(y / f));
	}

	template< class T >
	Vec operator*(T f) const
	{
		return Vec((float)(x * f), (float)(y * f));
	}

	template< class T >
	void operator*=(T f)
	{
		x *= f;
		y *= f;
	}

	void operator*=(Vec v)
	{
		x *= v.x;
		y *= v.y;
	}

	operator SDL_Point() const;

	operator IntVec() const;

	void Reset()
	{
		x = y = 0;
	}

	explicit operator bool()
	{
		return x != 0.0f || y != 0.0f;
	}

	float GetLength() const
	{
		return (float) sqrt(x*x + y * y);
	}

	float GetLengthSq() const
	{
		return x * x + y * y;
	}

	Vec GetNormalized() const
	{
		return *this / GetLength();
	}

	void Normalize()
	{
		float len = GetLength();
		x /= len;
		y /= len;
	}

	static Vec FromAngle(float fAngle)
	{
		return Vec(std::cosf(fAngle), std::sinf(fAngle));
	}
};

template < class T >
Vec operator*(T i, const Vec& a)
{
	return a * i;
};


class IntVec
{
public:
	int x, y;
	IntVec() : x(0), y(0) {};
	IntVec(int _x, int _y) : x(_x), y(_y) {};

	bool operator==(const IntVec& other) const
	{
		return x == other.x && y == other.y;
	}

	bool operator!=(const IntVec& other) const
	{
		return x != other.x || y != other.y;
	}

	void operator+=(const IntVec& other)
	{
		x += other.x;
		y += other.y;
	}

	IntVec operator+(const IntVec& other) const
	{
		return IntVec(x + other.x, y + other.y);
	}

	void operator-=(const IntVec& other)
	{
		x -= other.x;
		y -= other.y;
	}

	IntVec operator-(const IntVec& other) const
	{
		return IntVec(x - other.x, y - other.y);
	}

	template< class T >
	IntVec operator/(T f) const
	{
		return IntVec((int)( x / f),(int)( y / f));
	}

	template< class T >
	IntVec operator*(T f) const
	{
		return IntVec((int)(x * f), (int)(y * f));
	}

	IntVec operator-()
	{
		return IntVec(-x, -y);
	}

	IntVec operator*(const Vec& other) const
	{
		return IntVec((int)(x * other.x), (int)(y * other.y));
	}

	IntVec operator*(const IntVec& other) const
	{
		return IntVec(x * other.x, y * other.y);
	}

	explicit operator SDL_Point() const;

	operator Vec() const
	{
		return Vec((float)x, (float)y);
	}

	void Reset()
	{
		x = y = 0;
	}

	explicit operator bool()
	{
		return x != 0 || y != 0;
	}
};

class IntRect
{
public:
	IntRect() {};
	IntRect(int x, int y, int sizex, int sizey)
		:m_vPos(x, y), m_vSize(sizex, sizey) {}
	IntVec GetUpperLeft() const { return m_vPos; } 
	IntVec GetBottomRight() const { return m_vPos + m_vSize; }
	IntVec GetSize() const { return m_vSize; }
	void SetPos(const IntVec& pos) { m_vPos = pos; }
	void SetSize(const IntVec& size) { m_vSize = size; }

	bool HasIntersection(const IntRect& other) const;
	bool IsInside(IntVec pos) const;

	operator SDL_Rect() const;
	IntRect(const SDL_Rect& rect);

	template< class T >
	IntRect operator*(T f) const
	{
		return IntRect((int)(m_vPos.x * f), (int)(m_vPos.y * f), (int)(m_vSize.x * f), (int)(m_vSize.y * f));
	}

private:
	IntVec m_vPos, m_vSize;
};
