#include "SDL.h"
#include "Input.h"
#include "Time.h"
#include <iostream>

std::unordered_map<int, SToggleState> Input::m_aButtonStates;
std::set<int> Input::m_aJustPressed;
std::set<int> Input::m_aJustReleased;
std::set<int> Input::m_aPressed;
IntVec Input::m_vMousePos;

class SDLEventHandler
{
	static void Handle(const SDL_Event& Event)
	{
		switch (Event.type)
		{
		case SDL_KEYDOWN:
		{
			if (!Event.key.repeat)
			{
				int key = Event.key.keysym.scancode;
				SToggleState& State = Input::GetButton(key);
				State.active = true;
				State.downtime = 0.0f;
				State.pressed = true;
				Input::m_aJustPressed.insert(key);
				//std::cout << key << "\n";
			}
		}
		break;
		case SDL_KEYUP:
		{
			int key = Event.key.keysym.scancode;
			SToggleState& State = Input::GetButton(key);
			State.active = false;
			State.released = true;
			Input::m_aJustReleased.insert(key);
		}
		break;
		case SDL_MOUSEMOTION:
			Input::m_vMousePos.x = Event.motion.x;
			Input::m_vMousePos.y = Event.motion.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
		{
			int key = -1 * Event.button.button;
			SToggleState& State = Input::GetButton(key);
			State.active = true;
			State.downtime = 0.0f;
			State.pressed = true;
			Input::m_aJustPressed.insert(key);
		}
		break;
		case SDL_MOUSEBUTTONUP:
		{
			int key = -1 * Event.button.button;
			SToggleState& State = Input::GetButton(key);
			State.active = false;
			State.released = true;
			Input::m_aJustReleased.insert(key);
		}
		break;
		case SDL_MOUSEWHEEL:
			break;
		default:
			break;
		}
	}
};


SToggleState& Input::GetButton(int key)
{
	auto search = m_aButtonStates.find(key);
	if (search == m_aButtonStates.end()) {
		SToggleState state;
		auto it =m_aButtonStates.insert({ key,state });
		return it.first->second;
	}
	else {
		return search->second;
	}
}

void Input::Update()
{
	for (auto&& it : m_aJustReleased)
	{
		m_aButtonStates[it].released = false;
		m_aPressed.erase(it);
	}
	for (auto&& it : m_aJustPressed)
	{
		m_aButtonStates[it].pressed = false;
		m_aPressed.insert(it);
	}
	m_aJustReleased.clear();
	m_aJustPressed.clear();

	for (auto&& it : m_aPressed)
	{
		m_aButtonStates[it].downtime += (float)Time::frame;
	}
}

const SToggleState& Input::GetKey(int key)
{
	return GetButton(key);
}
