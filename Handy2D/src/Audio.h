#pragma once
#include "Sound.h"

class CAudio
{
public:
	static void PlaySound(std::weak_ptr<CSound> pSound, int nChannel = -1);
	static void AllocateChannels(int nCount);
};