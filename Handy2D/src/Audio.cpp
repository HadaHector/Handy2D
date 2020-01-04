#include "Audio.h"
#include "SDL_mixer.h"

void CAudio::PlaySound(std::weak_ptr<CSound> pSound, int nChannel)
{
	if (pSound.expired()) return;
	Mix_PlayChannel(nChannel, pSound.lock()->GetChunk(), 0);
}

void CAudio::AllocateChannels(int nCount)
{
	Mix_AllocateChannels(nCount);
}

void CAudio::SetVolume(int nChannel, int nVolume)
{
	Mix_Volume(nChannel, nVolume);
}

