#ifndef _TESTPLAYER_H_
#define _TESTPLAYER_H_

#include "Object.h"


class TestPlayer {
private:
	DirectX::XMFLOAT3 m_position;
	std::unique_ptr<ModelObject> m_modelObject;

public:
	TestPlayer();

	DirectX::XMFLOAT3 GetPosition() { return m_position; }

	bool Run();
};

#endif