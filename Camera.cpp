#include <math.h>
#include <Windows.h>

#include "Camera.h"
using namespace DirectX;


bool Camera::Run() {
	static float x, y, z;

	if (GetAsyncKeyState(VK_LEFT))
		m_horizontal -= 0.03f;
	if (GetAsyncKeyState(VK_RIGHT))
		m_horizontal += 0.03f;
	if (GetAsyncKeyState(VK_UP))
		m_vertical += 0.03f;
	if (GetAsyncKeyState(VK_DOWN))
		m_vertical -= 0.03f;

	if (GetAsyncKeyState('W')) {
		x -= sinf(m_horizontal) * 0.15f;
		z -= cosf(m_horizontal) * 0.15f;
	}
	if (GetAsyncKeyState('S')) {
		x += sinf(m_horizontal) * 0.15f;
		z += cosf(m_horizontal) * 0.15f;
	}
	if (GetAsyncKeyState('A')) {
		x += cosf(m_horizontal) * 0.15f;
		z -= sinf(m_horizontal) * 0.15f;
	}
	if (GetAsyncKeyState('D')) {
		x -= cosf(m_horizontal) * 0.15f;
		z += sinf(m_horizontal) * 0.15f;
	}
	if (GetAsyncKeyState(VK_LSHIFT)) {
		y += 0.1f;
	}
	if (GetAsyncKeyState(VK_LCONTROL)) {
		y -= 0.1f;
	}

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(
		XMVectorSet(
			sinf(m_horizontal) * cosf(m_vertical) * 4.0f + x,
			sinf(m_vertical) * 4.0f + y,
			cosf(m_horizontal) * cosf(m_vertical) * 4.0f + z, 0.0f),
		XMVectorSet(x, y + 0.001f, z, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));

	XMStoreFloat4x4(&m_viewProjMatrix, XMLoadFloat4x4(&m_viewMatrix) * XMLoadFloat4x4(&m_projMatrix));

	return true;
}