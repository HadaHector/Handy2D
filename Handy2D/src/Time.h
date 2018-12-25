#pragma once
class Time
{
public:
	static double frame;
	static double full;
	static unsigned long long frameNumber;
	static int fps;
	static float fpsraw;

	static void Init();
	static void FrameBegin();
	static void FrameEnd();
	static void Close();
};

