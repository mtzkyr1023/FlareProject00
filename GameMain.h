#ifndef _GAMEMAIN_H_
#define _GAMEMAIN_H_

#include "Scene.h"

class GameMain {
private:
	GameScene m_scene;


public:
	GameMain() {}

	bool Init(HWND hwnd, int width, int height);

	bool Run();
};

#endif