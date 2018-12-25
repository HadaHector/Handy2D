#pragma once
#include <unordered_map>
#include <set>
#include "Geometry.h"
#include "Keys.h"

#define MOUSE_LEFT -1
#define MOUSE_RIGHT -3
#define MOUSE_MIDDLE -2

struct SToggleState //every key or button that has on/off states (keyboard, mouse buttons)
{
	bool active = false; //currently held down
	bool pressed = false; //it was pressed in this frame
	bool released = false; //it was released in this frame
	float downtime = 0.0f; //this long it is held down currently or last time
};


class Input
{
	static std::unordered_map<int, SToggleState> m_aButtonStates;
	static SToggleState& GetButton(int key);
	static std::set<int> m_aJustPressed;
	static std::set<int> m_aJustReleased;
	static std::set<int> m_aPressed;
	static IntVec m_vMousePos;

	friend class SDLEventHandler;
public:
	static void Update();
	static const SToggleState& GetKey(int key);
	static IntVec GetMousePos() { return m_vMousePos; }

};

