#include "Time.h"
#include <chrono>
#include <thread>
#include <Windows.h>

#pragma comment(lib,"Winmm.lib") 

class HeighResolutionClockKeeper
{
private:
	bool using_higher_res_timer;
public:
	HeighResolutionClockKeeper() : using_higher_res_timer(false) {}
	void QueryHeighResolutionClock()
	{
		if (timeBeginPeriod(1) != TIMERR_NOCANDO)
		{
			using_higher_res_timer = true;
		}
	}
	void FreeHeighResolutionClock()
	{
		if (using_higher_res_timer)
		{
			timeEndPeriod(1);
		}
	}
	~HeighResolutionClockKeeper()
	{
		FreeHeighResolutionClock(); /// if exception is thrown , if not this wont cause problems thanks to the flag we put
	}
};

double Time::frame = 0;
double Time::full = 0;
unsigned long long Time::frameNumber = 0;
float Time::fpsraw = 0.0f;
int Time::fps = 0;
float Time::target_fps = 60;

namespace
{
	HeighResolutionClockKeeper MyHeighResolutionClockKeeper;

	using sys_clock = std::chrono::high_resolution_clock;
	using fsecond = std::chrono::duration<float>;

	std::chrono::steady_clock::time_point game_start;
	std::chrono::steady_clock::time_point start_point;
	std::chrono::steady_clock::time_point frame_begin, frame_end, after;

	float tmp_target_fps = Time::target_fps;
	auto target_frame_len = fsecond(1.0f / 60);
	bool enable_fps_oscillation = true;
	float accum_fps = 0.0f;
	int frames_count = 0;
}



void Time::Init()
{
	MyHeighResolutionClockKeeper.QueryHeighResolutionClock();
	game_start = sys_clock::now();
	start_point = sys_clock::now();
}

void Time::FrameBegin()
{
	frame_begin = std::chrono::high_resolution_clock::now();
}

void Time::FrameEnd()
{
	frame_end = std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(target_frame_len - (frame_end.time_since_epoch() - frame_begin.time_since_epoch()));
	after = std::chrono::high_resolution_clock::now();

	float _fps = fsecond(1) / (std::chrono::high_resolution_clock::now() - frame_begin); /// this will show ass the current FPS
	Time::fpsraw = _fps;
	Time::frame = std::chrono::duration<double>(after - frame_begin).count();
	Time::full = std::chrono::duration<double>(after - game_start).count();

	if (_fps < Time::target_fps) /// our real fps is less than what we want
		Time::target_fps += 0.03f; /// lets asl for more !
	else if (_fps > Time::target_fps) /// it is more than what we want
		Time::target_fps -= 0.03f; /// lets ask for less
	if (enable_fps_oscillation == true)
	{
		/// now we will adjust our target frame length for match the new target FPS
		target_frame_len = fsecond(1.0f / tmp_target_fps);
		/// used to calculate average FPS
		accum_fps += _fps;
		frames_count++;
		/// each 1 second
		if ((after - start_point)>fsecond(0.1f)) /// show average each 1 sec
		{
			start_point = sys_clock::now();
			fps = (int)roundf(accum_fps / frames_count);
		}
	}
}

void Time::Close()
{
	MyHeighResolutionClockKeeper.FreeHeighResolutionClock();
}