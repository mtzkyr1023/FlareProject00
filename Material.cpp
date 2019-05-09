#include <string>

#include "WICTextureLoader.h"
#include "Material.h"

using namespace DirectX;
using namespace std;

bool Material::LoadTexture(const char* filename) {
	bool res;
	wstring name = L"tex/";
	wchar_t* tmp = new wchar_t[strlen(filename) * 2];
	size_t dummy;
	mbstowcs_s(&dummy, tmp, strlen(filename) * 2, filename, strlen(filename));
	name += tmp;
	ComPtr<ID3D11Resource> tex;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	res = CreateWICTextureFromFile(m_device.Get(), m_context.Get(), name.c_str(), tex.ReleaseAndGetAddressOf(),
		m_texture.ReleaseAndGetAddressOf());

	delete[] tmp;

	if(!res)
		return false;

	return true;
}