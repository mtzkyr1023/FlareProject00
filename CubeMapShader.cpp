#include "CubeMapShader.h"
using namespace std;

bool CubeMapVS::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT res;
	ComPtr<ID3DBlob> VSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(L"shader/cubemap_shader/cubemap_vs.fx", "vs_main", "vs_5_0", VSBlob.ReleaseAndGetAddressOf(),
		errorMessage.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "shader/cubemap_shader/cubemap_vs_");
		return false;
	}

	res = m_device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL,
		m_vertexShader.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating cubemap vs.", "CubeMapShader.cpp", MB_OK);
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	res = m_device->CreateInputLayout(layout, ARRAYSIZE(layout), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(),
		m_layout.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating cubemap input layout.", "CubeMapShader.cpp", MB_OK);
		return false;
	}

	return true;
}


bool CubeMapPS::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT res;
	ComPtr<ID3DBlob> PSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(L"shader/cubemap_shader/cubemap_ps.fx", "ps_main", "ps_5_0",
		PSBlob.ReleaseAndGetAddressOf(), errorMessage.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "shader/cubemap_shader/cubemap_ps_");
		return false;
	}

	res = m_device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL,
		m_pixelShader.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating cube map ps.", "CubeMapShader.cpp", MB_OK);
		return false;
	}

	return true;
}