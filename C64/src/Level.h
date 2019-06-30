#pragma once
#include <vector>

class CLevel
{
public:
	void Load(const std::string& sFile);
	std::vector<char> m_aBricks;
};

