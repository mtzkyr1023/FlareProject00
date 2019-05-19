#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <DirectXMath.h>

class Camera {
protected:
	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projMatrix;
	DirectX::XMFLOAT4X4 m_viewProjMatrix;
	DirectX::XMFLOAT4X4 m_invViewMatrix;

	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_focus;

	float m_horizontal;
	float m_vertical;

public:
	Camera() {
		DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(4.5f, 4.5f, 4.5f, 0.0f),
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
		DirectX::XMStoreFloat4x4(&m_projMatrix, DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 4.0f / 3.0f,
			0.1f, 500.0f));

		m_position = DirectX::XMFLOAT3(34.5f, 4.5f, 4.5f);
		m_focus = DirectX::XMFLOAT3(30.0f, 0.0f, 0.0f);

		m_horizontal = 0.0f;
		m_vertical = 0.0f;
	}

	virtual ~Camera() {}

	DirectX::XMFLOAT4X4* GetViewMatrix() { return &m_viewMatrix; }
	DirectX::XMFLOAT4X4* GetProjMatrix() { return &m_projMatrix; }
	DirectX::XMFLOAT4X4* GetViewProjMatrix() { return &m_viewProjMatrix; }
	DirectX::XMFLOAT4X4* GetInvViewMatrix() { return &m_invViewMatrix; }

	bool Run();
};

#endif