#include <string>
using namespace std;

#include "Environment.h"
#include "stb_lib/stb_image.h"

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