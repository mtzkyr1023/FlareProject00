#ifndef _SCENE_H_
#define _SCENE_H_

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class SceneBase {
public:
	enum StateResult {
		StateResult_Finish,
		StateResult_Continue,
	};

public:
	SceneBase() {}
	virtual ~SceneBase() {}

	virtual StateResult Update() = 0;
};


class TitleScene : public SceneBase {
public:
	enum Select {
		Select_None,
		Select_Game,
		Select_Exit,
	};

private:
	void (TitleScene::*m_stateFunc)(unsigned int);
	StateResult m_stateResult;
	Select m_select;

private:
	void init(unsigned int dt);
	void idle(unsigned int dt);
	void end(unsigned int dt);

	void SetState(void(TitleScene::*stateFunc)(unsigned int)) {
		m_stateFunc = stateFunc;
	}

public:
	TitleScene() : m_stateResult(StateResult_Continue), m_select(Select_None) {
		SetState(&TitleScene::init);
	}
	virtual ~TitleScene() {}

	virtual StateResult Update() {
		(this->*m_stateFunc)(1);
		return m_stateResult;
	}

	virtual bool GetSelectButton();
	virtual bool GetCancelButton();

	Select GetSelectResult() { return m_select; }
};


class MyGameScene : public SceneBase {
private:
	void (MyGameScene::*m_stateFunc)(unsigned int dt);
	StateResult m_stateResult;

private:
	void init(unsigned int dt);
	void idle(unsigned int dt);
	void end(unsigned int dt);

	void SetState(void(MyGameScene::*stateFunc)(unsigned int)) {
		m_stateFunc = stateFunc;
	}

public:
	MyGameScene() : m_stateResult(StateResult_Continue) {
		m_stateFunc = &MyGameScene::init;
	}
	virtual ~MyGameScene() {}

	virtual StateResult Update() {
		(this->*m_stateFunc)(1);
		return m_stateResult;
	}
};

class GameScene : public SceneBase {
private:
	void (GameScene::*m_stateFunc)();
	StateResult m_stateResult;
	TitleScene m_titleScene;
	MyGameScene m_myGameScene;

private:
	void SetState(void (GameScene::*stateFunc)()) {
		m_stateFunc = stateFunc;
	}

public:
	GameScene() : m_stateResult(StateResult_Continue) {
		SetState(&GameScene::title);
	}

	virtual StateResult Update();

	void title();
	void game();
};

#endif