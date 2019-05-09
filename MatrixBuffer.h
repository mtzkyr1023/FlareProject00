#ifndef _MATRIXBUFFER_H_
#define _MATRIXBUFFER_H_

#include <DirectXMath.h>

#include "Device.h"
#include "Model.h"

class MatrixFactory : DeviceUser {
private:
	ComPtr<ID3D11Buffer> m_viewBuffer;
	ComPtr<ID3D11Buffer> m_projBuffer;
	ComPtr<ID3D11Buffer> m_vpBuffer;
	ComPtr<ID3D11Buffer> m_worldBuffer;
	ComPtr<ID3D11Buffer> m_shadowBuffer;
	ComPtr<ID3D11Buffer> m_timerBuffer;

public:
	bool Initialize();

	ComPtr<ID3D11Buffer> GetViewBuffer() { return m_viewBuffer; }
	ComPtr<ID3D11Buffer> GetProjBuffer() { return m_projBuffer; }
	ComPtr<ID3D11Buffer> GetViewProjBuffer() { return m_vpBuffer; }
	ComPtr<ID3D11Buffer> GetWorldBuffer() { return m_worldBuffer; }
	ComPtr<ID3D11Buffer> GetShadowBuffer() { return m_shadowBuffer; }
	ComPtr<ID3D11Buffer> GetTimeBuffer() { return m_timerBuffer; }

	void SetViewMatrix(DirectX::XMFLOAT4X4* matrix);
	void SetProjMatrix(DirectX::XMFLOAT4X4* matrix);
	void SetWorldMatrix(DirectX::XMFLOAT4X4* matrix);
	void SetVPMatrix(DirectX::XMFLOAT4X4* matrix);
	void CountTimer(float time);

	void SetShadowMatrix(DirectX::XMFLOAT4X4* view, DirectX::XMFLOAT4X4* proj);

	static MatrixFactory& Inst() {
		static MatrixFactory inst;
		return inst;
	}
};


#endif