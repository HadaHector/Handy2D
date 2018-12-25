#pragma once

struct SDL_Point;
struct SDL_Rect;

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

	Vec operator+(const Vec& other)
	{
		return Vec(x + other.x, y + other.y);
	}

	void operator-=(const Vec& other)
	{
		x -= other.x;
		y -= other.y;
	}

	Vec operator-(const Vec& other)
	{
		return Vec(x - other.x, y - other.y);
	}

	Vec operator-()
	{
		return Vec(-x, -y);
	}
	operator SDL_Point() const;
};


class IntVec
{
public:
	int x, y;
	IntVec() : x(0), y(0) {};
	IntVec(int _x, int _y) : x(_x), y(_y) {};

	void operator+=(const IntVec& other)
	{
		x += other.x;
		y += other.y;
	}

	IntVec operator+(const IntVec& other)
	{
		return IntVec(x + other.x, y + other.y);
	}

	void operator-=(const IntVec& other)
	{
		x -= other.x;
		y -= other.y;
	}

	IntVec operator-(const IntVec& other)
	{
		return IntVec(x - other.x, y - other.y);
	}

	IntVec operator-()
	{
		return IntVec(-x, -y);
	}
	explicit operator SDL_Point() const;

	operator Vec() const
	{
		return Vec((float)x, (float)y);
	}
};

class IntRect
{
public:
	IntRect() {};
	IntRect(int x, int y, int x2, int y2)
		:m_vPos(x, y), m_vSize(x2, y2) {}
	IntVec GetUpperLeft() { return m_vPos; }
	IntVec GetBottomRight() { return m_vPos + m_vSize; }
	IntVec GetSize() { return m_vSize; }

	operator SDL_Rect() const;
	IntRect(const SDL_Rect& rect);

private:
	IntVec m_vPos, m_vSize;
};
