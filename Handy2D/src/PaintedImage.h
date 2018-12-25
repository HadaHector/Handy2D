#pragma once
#include <SDL.h>
#include <functional>


class PaintedImage
{
private:
	int m_nWidth = 0, m_nHeight = 0;
	SDL_Surface* m_pSurface = nullptr;

public:
	PaintedImage(int nWidth, int nHeight);

	~PaintedImage();

	void Set(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	void Set(int x, int y, Uint32 rgba);

	void Fill(std::function<int(int, int)> fn);

	SDL_Surface* GetData() { return m_pSurface; }

};

