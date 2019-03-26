#ifndef _MATRIXBUFFER_H_
#define _MATRIXBUFFER_H_


#define INSTANCE_NUM 64

#include <DirectXMath.h>

#include "Device.h"
#include "Model.h"

class MatrixFactory : DeviceUser {
private:
	ComPtr<ID3D11Buffer> m_viewBuffer;
	ComPtr<ID3D11Buffer> m_projBuffer;
	ComPtr<ID3D11Buffer> m_worldBuffer;
	ComPtr<ID3D11ShaderResourceView> m_worldView;

	DirectX::XMFLOAT4X4* m_worldMatrixArray[MODEL_NUM][INSTANCE_NUM];

public:
	bool Initialize();

	ComPtr<ID3D11Buffer> GetViewBuffer() { return m_viewBuffer; }
	ComPtr<ID3D11Buffer> GetProjBuffer() { return m_projBuffer; }
	ComPtr<ID3D11Buffer> GetWorldBuffer() { return m_worldBuffer; }

	ComPtr<ID3D11ShaderResourceView> GetWorldView(int modelNum);

	void SetViewMatrix(DirectX::XMFLOAT4X4* matrix);
	void SetProjMatrix(DirectX::XMFLOAT4X4* matrix);
	void SetWorldMatrix(int modelNum, int elemNum, DirectX::XMFLOAT4X4* matrix);

	static MatrixFactory& Inst() {
		static MatrixFactory inst;
		return inst;
	}
};


#endif