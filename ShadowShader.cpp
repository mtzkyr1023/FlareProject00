#include "ShadowShader.h"


bool ShadowVS::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT res;
	ComPtr<ID3DBlob> VSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(L"shader/shadow_shader/shadow_vs.fx", "vs_static_main", "vs_5_0", VSBlob.GetAddressOf(),
		errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "shadow_shader/");
		return false;
	}

	res = m_device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, m_vertexShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating shadow vs.", "ShadowShader.cpp", MB_OK);
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	res = m_device->CreateInputLayout(layout, ARRAYSIZE(layout), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), m_layout.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating input layout.", "ModelVS.cpp", MB_OK);
		return false;
	}

	return true;
}


bool ShadowPS::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT res;
	ComPtr<ID3DBlob> PSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(L"shader/shadow_shader/shadow_ps.fx", "vs_static_main", "vs_5_0", PSBlob.GetAddressOf(),
		errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "shadow_shader/");
		return false;
	}

	res = m_device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, m_pixelShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating shadow vs.", "ShadowShader.cpp", MB_OK);
		return false;
	}

	return true;
}