#include "SDLManager.h"
#include "GuiLayer.h"
#include "PaintedImage.h"
#include "Input.h"
#include <Windows.h>
#include <random>


namespace {
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_real_distribution<float> random_float(0, 1);
	std::uniform_int_distribution<int> random_bool(0, 1);
}


struct Vec3D
{
	float x, y, z;
	Vec3D() : x(0.0f), y(0.0f), z(0.0f) {}
	Vec3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};

	float dot(const Vec3D& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	float length() const
	{
		return sqrt(x*x + y * y + z * z);
	}

	Vec3D normalized() const
	{
		float len = length();
		return Vec3D(x / len, y / len, z / len);
	}

	Vec3D operator+(const Vec3D& other) const
	{
		return Vec3D(x + other.x, y + other.y, z + other.z);
	}

	Vec3D operator-(const Vec3D& other) const
	{
		return Vec3D(x - other.x, y - other.y, z - other.z);
	}

};


float Random(int seed, int x, int y)
{
	float f = (seed + (seed + x * 234523 + y * 98457) * x * 2456 * y * 45637) / 789.657234f;
	return f - floor(f);
}

float PerlinOctave(int seed, Vec vec)
{
	int x0 = floor(vec.x);
	int y0 = floor(vec.y);
	int x1 = floor(vec.x+1);
	int y1 = floor(vec.y+1);

	float a = vec.x - x0;
	float c = vec.y - y0;

	float v00 = Random(seed, x0, y0);
	float v01 = Random(seed, x0, y1);
	float v10 = Random(seed, x1, y0);
	float v11 = Random(seed, x1, y1);

	float b1 = v00 * (1 - a) + v10 * a;
	float b2 = v01 * (1 - a) + v11 * a;
	return b1 * (1 - c) + b2 * c;
}



class CSpaceExplorer : public CGameLogic
{
public:
	// Inherited via CGameLogic
	virtual bool Load() override
	{
		Time::target_fps = 120;

		m_pGui = new CGuiLayer(IntRect(0, 0, 1024, 512));
		SDLManager::Instance.AddLayer(m_pGui);

		m_pPlanetImage = PNEW(CGuiImage);
		m_pPlanetImage->SetSize(IntVec(1024, 512));
		m_pPlanetImage->SetPosition(IntVec(0, 0));
		m_pGui->GetRootElement()->AddChild(m_pPlanetImage);

		m_pPlanetTex = new CPaintedImage(1024, 512);

		m_vSundir = Vec3D(0, 0, 0.4);

		DrawPlanet();

		return true;
	}

	virtual void Update() override
	{
		if (Input::GetKey(KEY_SPACE).pressed)
		{
			m_vSundir = Vec3D(random_float(e1) * 2 - 1, random_float(e1) * 2 - 1, random_float(e1) * 2 - 1);
			DrawPlanet();
		}

		if (Input::GetKey(KEY_KP_PLUS).pressed)
		{
			limit += 0.1f;
			DrawPlanet();
		}

		if (Input::GetKey(KEY_KP_MINUS).pressed)
		{
			limit -= 0.1f;
			DrawPlanet();
		}
	}

	void DrawPlanet()
	{
		const int space = 0x000000FF;
		const float radius = 1900;
		const Vec3D sundir = m_vSundir.normalized();

		const int seed1 = random_float(e1) * 10000;
		const int seed2 = random_float(e1) * 10000;
		const int seed3 = random_float(e1) * 10000;

		const int cloud1 = random_float(e1) * 10000;
		const int cloud2 = random_float(e1) * 10000;

		m_pPlanetTex->Fill([=](int x, int y) -> int {
			Vec camera(x-512, y-2048);
			Vec origo(0, 0);
			float dist = (camera - origo).GetLength();
			if (dist > radius) {
				return space;
			}
			else
			{
				Vec3D vSurface(camera.x, camera.y, radius * cosf(dist / radius));
				Vec3D normal = vSurface.normalized();
				
				Vec vUV( atanf(normal.x / normal.y), normal.z*3);



				float light = max(0.01f, pow(0.5f, max(0.0f, normal.dot(sundir))));
				float height = (PerlinOctave(seed1, vUV * 15.0f) + 0.4 * PerlinOctave(seed2, vUV * 30) + 0.1 * PerlinOctave(seed3, vUV * 120.0f));
				float cloud = (PerlinOctave(cloud1, vUV * 30.0f) + 0.4 * PerlinOctave(cloud2, vUV * (60 + 10 * PerlinOctave(seed3, vUV * 100.0f)) ) );
				
				Color col;
				if (cloud > 0.75)
				{
					col = Color(255, 255, 255, 255);
				}
				else if (height > limit)
				{
					col = Color((103 + height * 75), (143 + height * 75), 44, 255);
				}
				else
				{
					col = Color(27, 91, 243, 255);
				}

				col.r *= light;
				col.g *= light;
				col.b *= light;
				return col;			
			}
		});

		CTexture::DelTexture("planet");
		m_pPlanetTex->CreateTexture("planet");
		m_pPlanetImage->SetImage("planet");
	}

	CGuiLayer* m_pGui;
	CPaintedImage* m_pPlanetTex;
	PGuiImage m_pPlanetImage;

	Vec3D m_vSundir;

	float limit = 0.5f;

};





int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{

	SWindowParams Params;
	Params.nWidth = 1024;
	Params.nHeight = 512;
	Params.sTitle = "Space Explorer";
	Params.bResizeAble = false;
	Params.bMaximized = false;

	CSpaceExplorer* pGame = new CSpaceExplorer();
	SDLManager::Start(Params, pGame);
	//delete pGame;

	return 0;
}