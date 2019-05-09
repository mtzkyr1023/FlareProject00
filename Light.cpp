#include "Light.h"
using namespace DirectX;

bool DirectionalLight::Run() {
	static float horizontal, vertical = XM_PIDIV4;


	horizontal += XM_PI / 360.0f;

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(
		XMVectorSet(
			sinf(horizontal) * cosf(vertical) * 4.0f,
			sinf(vertical) * 4.0f,
			cosf(horizontal) * cosf(vertical) * 4.0f, 0.0f),
		XMVectorSet(0.0f, 0.001f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));

	return true;
}