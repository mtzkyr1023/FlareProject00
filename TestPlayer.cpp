#include "TestPlayer.h"
using namespace std;
using namespace DirectX;


TestPlayer::TestPlayer() {
	m_modelObject.reset(new ModelObject(0));
}

bool TestPlayer::Run() {
	XMFLOAT4X4* mat = m_modelObject->GetMatrix();

	XMFLOAT3 axisX, axisY, axisZ;

	static float rot1, rot2, rot3;
	axisX = XMFLOAT3(mat->_11, mat->_12, mat->_13);
	axisY = XMFLOAT3(mat->_21, mat->_22, mat->_23);
	axisZ = XMFLOAT3(mat->_31, mat->_32, mat->_33);
	axisX = XMFLOAT3(1, 0, 0);
	axisY = XMFLOAT3(0, 1, 0);
	axisZ = XMFLOAT3(0, 0, 1);

//	XMStoreFloat3(&axisX, XMVector3Cross(XMLoadFloat3(&axisZ), XMLoadFloat3(&axisY)));

	XMVECTOR q1 = XMQuaternionRotationAxis(XMLoadFloat3(&axisZ), rot1);
	XMVECTOR q2 = XMQuaternionRotationAxis(XMLoadFloat3(&axisX), rot2);
	XMVECTOR q3 = XMQuaternionNormalize(XMQuaternionMultiply(q2, q1));

	if (GetAsyncKeyState('Z'))
		rot1 -= 0.01f;
	if (GetAsyncKeyState('C'))
		rot1 += 0.01f;

	if (GetAsyncKeyState(VK_UP))
		rot2 += 0.01f;
	if (GetAsyncKeyState(VK_DOWN))
		rot2 -= 0.01f;

	XMStoreFloat4x4(mat, XMMatrixRotationQuaternion(q3));

	m_position.x -= mat->_31 * 0.01f;
	m_position.y -= mat->_32 * 0.01f;
	m_position.z -= mat->_33 * 0.01f;

	mat->_41 = m_position.x;
	mat->_42 = m_position.y;
	mat->_43 = m_position.z;

	return true;
}