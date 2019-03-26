#include "Model.h"
using namespace std;
using namespace DirectX;


bool Model::Initialize(const char* filename) {
	DefaultVertex vertex[3] = {
		{ XMFLOAT3(0.0f, 2.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-2.0f, -2.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(2.0f, -2.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	};

	int index[3] = { 0, 1, 2 };

	HRESULT res;
	D3D11_BUFFER_DESC bufDesc{};
	D3D11_SUBRESOURCE_DATA initData{};

	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.ByteWidth = sizeof(DefaultVertex) * 3;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.StructureByteStride = 0;
	bufDesc.MiscFlags = 0;

	initData.pSysMem = vertex;

	res = m_device->CreateBuffer(&bufDesc, &initData, m_vertexBuffer.GetAddressOf());


	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.ByteWidth = sizeof(int) * 3;
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.StructureByteStride = 0;
	bufDesc.MiscFlags = 0;

	initData.pSysMem = index;


	res = m_device->CreateBuffer(&bufDesc, &initData, m_indexBuffer.GetAddressOf());

	return true;
}


bool SkinnedModel::Initialize(const char* filename) {
	return true;
}

void SkinnedModel::SetBuffer() {
	UINT offset = 0;
	UINT stride[] = {
		m_stride1,
		m_stride2,
	};

	ID3D11Buffer* vertexBuffer[] = {
		m_vertexBuffer.Get(),
		m_boneVertexBuffer.Get(),
	};

	m_context->IASetVertexBuffers(0, 2, vertexBuffer, stride, &offset);
	m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}