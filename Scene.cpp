#include <Windows.h>


#include "Scene.h"


void TitleScene::init(unsigned int dt) {
	m_stateResult = StateResult_Continue;
	SetState(&TitleScene::idle);
}

void TitleScene::idle(unsigned int dt) {
	if (GetSelectButton()) {
		m_select = Select_Game;
		SetState(&TitleScene::end);
		return;
	}
	if (GetCancelButton()) {
		m_select = Select_Exit;
		SetState(&TitleScene::end);
		return;
	}
}

void TitleScene::end(unsigned int dt) {
	m_stateResult = StateResult_Finish;
	SetState(&TitleScene::init);
}

bool TitleScene::GetSelectButton() {
	return GetAsyncKeyState(VK_SPACE) & 0x0001;
}

bool TitleScene::GetCancelButton() {
	return GetAsyncKeyState(VK_ESCAPE) & 0x0001;
}

void MyGameScene::init(unsigned int dt) {
	m_stateResult = StateResult_Continue;
	SetState(&MyGameScene::idle);
}

void MyGameScene::idle(unsigned int dt) {
	if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
		SetState(&MyGameScene::end);
}

void MyGameScene::end(unsigned int dt) {
	m_stateResult = StateResult_Finish;
	SetState(&MyGameScene::init);
}

SceneBase::StateResult GameScene::Update() {
	(this->*m_stateFunc)();

	return m_stateResult;
}

void GameScene::title() {
	if (m_titleScene.Update() == StateResult_Finish) {
		switch (m_titleScene.GetSelectResult()) {
		case TitleScene::Select_Game:
			SetState(&GameScene::game);
			break;

		case TitleScene::Select_Exit:
			SetState(&GameScene::title);
			m_stateResult = StateResult_Finish;
			break;
		}
	}
}

void GameScene::game() {
	if (m_myGameScene.Update() == StateResult_Finish) {
		SetState(&GameScene::title);
	}
}