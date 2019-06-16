#pragma once
#include <functional>

struct SDL_Surface;
struct STextureRef;

class CPaintedImage
{
private:
	int m_nWidth = 0, m_nHeight = 0;
	SDL_Surface* m_pSurface = nullptr;

public:
	CPaintedImage(int nWidth, int nHeight);

	~CPaintedImage();

	void Set(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	void Set(int x, int y, unsigned int rgba);

	void Fill(std::function<int(int, int)> fn);

	STextureRef CreateTexture(const std::string& sName);

	SDL_Surface* GetData() { return m_pSurface; }

};

