#include "GameMain.h"


bool GameMain::Init(HWND hwnd, int width, int height) {

	return true;
}

bool GameMain::Run() {
	if (m_scene.Update() == GameScene::StateResult_Finish)
		return false;

	return true;
}