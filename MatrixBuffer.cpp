

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

	res = m_device->CreateBuffer(&bufDesc, NULL, m_worldBuffer.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating world matrix buffer.", "MatrixBuffer.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateBuffer(&bufDesc, NULL, m_vpBuffer.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating world matrix buffer.", "MatrixBuffer.cpp", MB_OK);
		return false;
	}

	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.ByteWidth = sizeof(XMFLOAT4X4) * 2;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.StructureByteStride = 0;
	bufDesc.MiscFlags = 0;

	res = m_device->CreateBuffer(&bufDesc, NULL, m_shadowBuffer.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating shadow buffer.", "MatrixBuffer.cpp", MB_OK);
		return false;
	}

	D3D11_SUBRESOURCE_DATA initData{};

	XMFLOAT4 timer = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.ByteWidth = sizeof(XMFLOAT4);
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.StructureByteStride = 0;
	bufDesc.MiscFlags = 0;
	initData.pSysMem = &timer;
	res = m_device->CreateBuffer(&bufDesc, NULL, m_timerBuffer.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating time buffer.", "MatrixBuffer.cpp", MB_OK);
		return false;
	}

	return true;
}

void MatrixFactory::SetViewMatrix(XMFLOAT4X4* matrix) {
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMFLOAT4X4* ptr;

	res = m_context->Map(m_viewBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(res)) {
		ptr = (XMFLOAT4X4*)mappedResource.pData;

		XMStoreFloat4x4(ptr, XMMatrixTranspose(XMLoadFloat4x4(matrix)));

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

		XMStoreFloat4x4(ptr, XMMatrixTranspose(XMLoadFloat4x4(matrix)));

		m_context->Unmap(m_projBuffer.Get(), 0);
	}
}

void MatrixFactory::SetWorldMatrix(XMFLOAT4X4* matrix) {
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMFLOAT4X4* ptr;

	res = m_context->Map(m_worldBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(res)) {
		ptr = (XMFLOAT4X4*)mappedResource.pData;

		XMStoreFloat4x4(ptr, XMMatrixTranspose(XMLoadFloat4x4(matrix)));

		m_context->Unmap(m_worldBuffer.Get(), 0);
	}
}

void MatrixFactory::SetVPMatrix(XMFLOAT4X4* matrix) {
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMFLOAT4X4* ptr;

	res = m_context->Map(m_vpBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(res)) {
		ptr = (XMFLOAT4X4*)mappedResource.pData;

		XMStoreFloat4x4(ptr, XMMatrixTranspose(XMLoadFloat4x4(matrix)));

		m_context->Unmap(m_vpBuffer.Get(), 0);
	}
}

void MatrixFactory::SetShadowMatrix(XMFLOAT4X4* view, XMFLOAT4X4* proj) {
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMFLOAT4X4* ptr;

	res = m_context->Map(m_shadowBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(res)) {
		ptr = (XMFLOAT4X4*)mappedResource.pData;

		XMStoreFloat4x4(ptr, XMMatrixTranspose(XMLoadFloat4x4(view)));
		ptr++;
		XMStoreFloat4x4(ptr, XMMatrixTranspose(XMLoadFloat4x4(proj)));

		m_context->Unmap(m_shadowBuffer.Get(), 0);
	}
}

void MatrixFactory::CountTimer(float time) {
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMFLOAT4* ptr;

	res = m_context->Map(m_timerBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(res)) {
		ptr = (XMFLOAT4*)mappedResource.pData;
		ptr->x++;

		m_context->Unmap(m_timerBuffer.Get(), 0);
	}
}