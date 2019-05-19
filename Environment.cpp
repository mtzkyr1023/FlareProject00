#include <string>
using namespace std;

#include "Environment.h"
#include "stb_lib/stb_image.h"
using namespace DirectX;

bool Environment::Initialize() {
	HRESULT res;
	D3D11_BUFFER_DESC bufDesc{};

	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.ByteWidth = sizeof(PBR_Type);
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	res = m_device->CreateBuffer(&bufDesc, NULL, m_materialBuffer.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating material buffer.", "Environment.cpp", MB_OK);
		return false;
	}

	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.ByteWidth = sizeof(DirectionalLightType);
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	res = m_device->CreateBuffer(&bufDesc, NULL, m_dirLightBuffer.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating directional light buffer.", "Environment.cpp", MB_OK);
		return false;
	}

	return true;
}

bool Environment::LoadCubeMap(const char* filename) {
	HRESULT res;
	D3D11_TEXTURE2D_DESC texDesc{};
	D3D11_SUBRESOURCE_DATA initData[6];
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	ComPtr<ID3D11Texture2D> tex;
	string foldername = filename;
	const char* names[6] = {
		"posx.jpg",
		"negx.jpg",
		"posy.jpg",
		"negy.jpg",
		"posz.jpg",
		"negz.jpg",
	};

	int width[6];
	int height[6];
	BYTE* pixels[6];
	int bpp;
	size_t nameEnd = foldername.size();

	for (int i = 0; i < 6; i++) {
		foldername.insert(nameEnd, names[i]);
		pixels[i] = stbi_load(foldername.c_str(), &width[i], &height[i], &bpp, 4);

		initData[i].pSysMem = pixels[i];
		initData[i].SysMemPitch = width[i] * 4;
		initData[i].SysMemSlicePitch = 0;

		foldername.replace(nameEnd, foldername.size(), "");
	}

	texDesc.Width = width[0];
	texDesc.Height = height[0];
	texDesc.ArraySize = 6;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = 1;
	srvDesc.TextureCube.MostDetailedMip = 0;

	res = m_device->CreateTexture2D(&texDesc, &initData[0], tex.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating cube tex.", "Environment.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateShaderResourceView(tex.Get(), &srvDesc, m_cubeMapSrv.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating cube srv.", "Environment,cpp", MB_OK);
		return false;
	}

	for (int i = 0; i < 6; i++)
		stbi_image_free(pixels[i]);

	return true;
}


void Environment::SetMaterial(XMFLOAT4 albedo, float metalness, float roughness) {
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	PBR_Type* ptr;

	res = m_context->Map(m_materialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(res)) {
		ptr = (PBR_Type*)mappedResource.pData;
		ptr->albedo = albedo;
		ptr->metalness = metalness;
		ptr->roughness = roughness;

		m_context->Unmap(m_materialBuffer.Get(), 0);
	}
}

void Environment::SetDirectionalLight(XMFLOAT3 direction, XMFLOAT3 color, XMFLOAT4X4* viewMatrix) {
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DirectionalLightType* ptr;

	res = m_context->Map(m_dirLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(res)) {
		ptr = (DirectionalLightType*)mappedResource.pData;
		ptr->worldSpaceDirection = XMFLOAT4(direction.x, direction.y, direction.z, 0);
		ptr->color = XMFLOAT4(color.x, color.y, color.z, 1);
		XMVECTOR vec = XMVectorSet(direction.x, direction.y, direction.z, 0.0f);
		vec = XMVector4Transform(vec, XMLoadFloat4x4(viewMatrix));
		XMStoreFloat4(&ptr->viewSpaceDirection, vec);

		m_context->Unmap(m_dirLightBuffer.Get(), 0);
	}
}