#include <SDL.h>
#include "PaintedImage.h"
#include "Texture.h"

CPaintedImage::CPaintedImage(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_pSurface = SDL_CreateRGBSurface(0, m_nWidth, m_nHeight, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
}

CPaintedImage::~CPaintedImage()
{
	if (m_pSurface)
	{
		SDL_free(m_pSurface);
	}
}

void CPaintedImage::Set(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
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

void CPaintedImage::Set(int x, int y, unsigned int rgba)
{
	if (m_pSurface && x >= 0 && y >= 0 && x < m_pSurface->w && y < m_pSurface->h)
	{
		int idx = (m_pSurface->w * y + x) * 4;
		((unsigned char*)m_pSurface->pixels)[idx + 0] = ((unsigned char*)(&rgba))[3];
		((unsigned char*)m_pSurface->pixels)[idx + 1] = ((unsigned char*)(&rgba))[2];
		((unsigned char*)m_pSurface->pixels)[idx + 2] = ((unsigned char*)(&rgba))[1];
		((unsigned char*)m_pSurface->pixels)[idx + 3] = ((unsigned char*)(&rgba))[0];
	}
}

void CPaintedImage::Fill(std::function<int(int, int)> fn)
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
			((unsigned char*)m_pSurface->pixels)[idx + 0] = ((unsigned char*)(&rgba))[0];
			((unsigned char*)m_pSurface->pixels)[idx + 1] = ((unsigned char*)(&rgba))[1];
			((unsigned char*)m_pSurface->pixels)[idx + 2] = ((unsigned char*)(&rgba))[2];
			((unsigned char*)m_pSurface->pixels)[idx + 3] = ((unsigned char*)(&rgba))[3];
		}
}

void CPaintedImage::CreateTexture(const std::string& sName)
{
	CTexture::AddSurface(m_pSurface, sName);
}
