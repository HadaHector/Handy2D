#include "SDLManager.h"
#include "RatGame.h"



int main(int argc, char* args[])
{
	SWindowParams Params;
	Params.nWidth = 800;
	Params.nHeight = 600;
	Params.sTitle = "Rats";
	Params.bResizeAble = true;
	Params.bMaximized = true;

	{
		CRatGame* pGame = new CRatGame();
		SDLManager::Start(Params, pGame);
	}
	//system("pause");
	return 0;
}

//http://lazyfoo.net/tutorials/SDL/index.php