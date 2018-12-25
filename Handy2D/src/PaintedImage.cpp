#include "PaintedImage.h"

PaintedImage::PaintedImage(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_pSurface = SDL_CreateRGBSurface(0, m_nWidth, m_nHeight, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
}

PaintedImage::~PaintedImage()
{
	if (m_pSurface)
	{
		SDL_free(m_pSurface);
	}
}

void PaintedImage::Set(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if (m_pSurface && x >= 0 && y >= 0 && x < m_pSurface->w && y < m_pSurface->h)
	{
		int idx = (m_pSurface->w * y + x) * 4;
		((unsigned char*)m_pSurface->pixels)[idx + 0] = r;
		((unsigned char*)m_pSurface->pixels)[idx + 1] = g;
		((unsigned char*)m_pSurface->pixels)[idx + 2] = b;
		((unsigned char*)m_pSurface->pixels)[idx + 3] = a;
	}
}

void PaintedImage::Set(int x, int y, Uint32 rgba)
{
	if (m_pSurface && x >= 0 && y >= 0 && x < m_pSurface->w && y < m_pSurface->h)
	{
		int idx = (m_pSurface->w * y + x) * 4;
		((unsigned char*)m_pSurface->pixels)[idx + 0] = rgba;
		((unsigned char*)m_pSurface->pixels)[idx + 1] = rgba << 8;
		((unsigned char*)m_pSurface->pixels)[idx + 2] = rgba << 16;
		((unsigned char*)m_pSurface->pixels)[idx + 3] = rgba << 24;
	}
}

void PaintedImage::Fill(std::function<int(int, int)> fn)
{
	if (!m_pSurface || !fn)
	{
		return;
	}

	int idx, rgba;
	for (int y = 0; y<m_pSurface->h; ++y)
		for (int x = 0; x < m_pSurface->w; ++x)
		{
			idx = (m_pSurface->w * y + x) * 4;
			rgba = fn(x, y);
			((unsigned char*)m_pSurface->pixels)[idx + 0] = (unsigned char)(rgba >> 24);
			((unsigned char*)m_pSurface->pixels)[idx + 1] = (unsigned char)(rgba >> 16);
			((unsigned char*)m_pSurface->pixels)[idx + 2] = (unsigned char)(rgba >> 8);
			((unsigned char*)m_pSurface->pixels)[idx + 3] = (unsigned char)rgba;
		}
}
