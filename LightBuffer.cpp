#include <memory>
#include <random>

#include "LightBuffer.h"
#include "shader/shader_defines.h"
using namespace DirectX;
using namespace std;

bool LightBufferFactory::Initialize() {
	HRESULT res;
	D3D11_BUFFER_DESC bufDesc{};
	D3D11_SUBRESOURCE_DATA initData{};
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};

	random_device rnd;
	unique_ptr<PointLightType[]> initLight(new PointLightType[POINT_LIGHT_NUM]);
	for (int i = 0; i < POINT_LIGHT_NUM; i++) {
		initLight[i].position = XMFLOAT3((float)(rnd() % 40) - 20.0f, 0.5f, (float)(rnd() % 40) - 20.0f);
		initLight[i].viewPos = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		initLight[i].range = 6.0f;
		initLight[i].color.x = (float)(rnd() % 256) / 256;
		initLight[i].color.y = 1.0f - initLight[i].color.x;
		initLight[i].color.z = (float)(rnd() % 256) / 256;
		initLight[i].color.w = (float)(rnd() % 30) / 50.0f;
	}


	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.ByteWidth = sizeof(PointLightType) * POINT_LIGHT_NUM;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufDesc.StructureByteStride = sizeof(PointLightType);

	initData.pSysMem = initLight.get();
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	res = m_device->CreateBuffer(&bufDesc, &initData, m_pointBuffer.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating light buffer.", "LightBuffer.cpp", MB_OK);
		return false;
	}

	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = POINT_LIGHT_NUM;
//	srvDesc.Buffer.ElementWidth = 128;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.ElementOffset = 0;

	res = m_device->CreateShaderResourceView(m_pointBuffer.Get(), &srvDesc, m_pointSrv.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating light srv.", "LightBuffer.cpp", MB_OK);
		return false;
	}

	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = POINT_LIGHT_NUM;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;

	res = m_device->CreateUnorderedAccessView(m_pointBuffer.Get(), &uavDesc, m_pointUav.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating light uav.", "LightBuffer.cpp", MB_OK);
		return false;
	}

	return true;
}