#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <DirectXMath.h>

#include "Device.h"

struct PointLightType {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 viewPos;
	float range;
	DirectX::XMFLOAT4 color;
};


class DirectionalLight {
	struct LightMatrixType {
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projMatrix;
	};

private:
	DirectX::XMFLOAT3 m_direction;
	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projMatrix;

public:
	DirectionalLight() {
		m_direction = DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f);
		DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(16.0f, 16.0f, 16.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		));
		DirectX::XMStoreFloat4x4(&m_projMatrix, DirectX::XMMatrixOrthographicLH(10, 10,
			0.1f, 100.0f));
//		DirectX::XMStoreFloat4x4(&m_projMatrix, DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 4.0f / 3.0f, 0.1f, 100.0f));
	}

	DirectX::XMFLOAT4X4* GetViewMatrix() { return &m_viewMatrix; }
	DirectX::XMFLOAT4X4* GetProjMatrix() { return &m_projMatrix; }

	bool Run();
};


#endif