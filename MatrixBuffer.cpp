

#include "MatrixBuffer.h"
using namespace DirectX;


bool MatrixFactory::Initialize() {
	HRESULT res;
	D3D11_BUFFER_DESC bufDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.ByteWidth = sizeof(XMFLOAT4X4);
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.StructureByteStride = 0;
	bufDesc.MiscFlags = 0;

	res = m_device->CreateBuffer(&bufDesc, NULL, m_viewBuffer.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating view matrix buffer.", "MatrixBuffer.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateBuffer(&bufDesc, NULL, m_projBuffer.ReleaseAndGetAddressOf());
	if(FAILED(res)) {
		MessageBox(NULL, "failed creating proj matrix buffer.", "MatrixBuffer.cpp", MB_OK);
		return false;
	}

	bufDesc.ByteWidth = sizeof(XMFLOAT4X4) * INSTANCE_NUM;
	bufDesc.StructureByteStride = sizeof(XMFLOAT4X4);
	bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	res = m_device->CreateBuffer(&bufDesc, NULL, m_worldBuffer.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating world matrix buffer.", "MatrixBuffer.cpp", MB_OK);
		return false;
	}

	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = sizeof(XMFLOAT4X4);
	srvDesc.Buffer.NumElements = INSTANCE_NUM;

	res = m_device->CreateShaderResourceView(m_worldBuffer.Get(), &srvDesc, m_worldView.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating world resource view.", "MatrixBuffer.cpp", MB_OK);
		return false;
	}

	return true;
}


ComPtr<ID3D11ShaderResourceView> MatrixFactory::GetWorldView(int modelNum) {
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMFLOAT4X4* ptr;

	res = m_context->Map(m_viewBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(res)) {
		ptr = (XMFLOAT4X4*)mappedResource.pData;

		for (int i = 0; i < INSTANCE_NUM; i++) {
			memcpy_s(ptr++, sizeof(XMFLOAT4X4), m_worldMatrixArray[modelNum][i], sizeof(XMFLOAT4X4));
		}
		m_context->Unmap(m_viewBuffer.Get(), 0);
	}

	return m_worldView;
}


void MatrixFactory::SetViewMatrix(XMFLOAT4X4* matrix) {
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMFLOAT4X4* ptr;

	res = m_context->Map(m_viewBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(res)) {
		ptr = (XMFLOAT4X4*)mappedResource.pData;

		memcpy_s(ptr, sizeof(XMFLOAT4X4), matrix, sizeof(XMFLOAT4X4));

		m_context->Unmap(m_viewBuffer.Get(), 0);
	}
}


void MatrixFactory::SetProjMatrix(XMFLOAT4X4* matrix) {
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMFLOAT4X4* ptr;

	res = m_context->Map(m_projBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(res)) {
		ptr = (XMFLOAT4X4*)mappedResource.pData;

		memcpy_s(ptr, sizeof(XMFLOAT4X4), matrix, sizeof(XMFLOAT4X4));

		m_context->Unmap(m_projBuffer.Get(), 0);
	}
}

void MatrixFactory::SetWorldMatrix(int modelNum, int elemNum, XMFLOAT4X4* matrix) {
	m_worldMatrixArray[modelNum][elemNum] = matrix;
}