#include "Level.h"
#include <fstream>
#include <sstream>
#include "Breakout.h"

void CLevel::Load(const std::string& sFile)
{
	std::ifstream t(sFile);
	std::stringstream buffer;
	buffer << t.rdbuf();

	std::string sChars = buffer.str();

	m_aBricks.reserve(LEVEL_WIDTH * LEVEL_HEIGHT);

	int counter = 0;
	for (unsigned int i = 0; i < sChars.size(); ++i)
	{
		if (sChars[i] >= '0' && sChars[i] <= '9')
		{
			m_aBricks.push_back( sChars[i] - '0' );
			counter++;
			if (counter == LEVEL_WIDTH * LEVEL_HEIGHT) return;
		}
	}
	return;
}

