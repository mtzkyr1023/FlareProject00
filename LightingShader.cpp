#include "LightingShader.h"


bool LightCullShader::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT res;
	ComPtr<ID3DBlob> CSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(L"shader/lighting_shader/light_update_cs.fx", "cs_main", "cs_5_0",
		CSBlob.GetAddressOf(), errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "lighting_shader/");
		return false;
	}

	res = m_device->CreateComputeShader(CSBlob->GetBufferPointer(), CSBlob->GetBufferSize(), NULL, m_computeShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating light cull shader.", "LightingShader.cpp", MB_OK);
		return false;
	}

	return true;
}


bool LightingShader::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT res;
	ComPtr<ID3DBlob> CSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(L"shader/lighting_shader/light_cs.fx", "cs_main", "cs_5_0",
		CSBlob.GetAddressOf(), errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "lighting_shader/");
		return false;
	}

	res = m_device->CreateComputeShader(CSBlob->GetBufferPointer(), CSBlob->GetBufferSize(), NULL, m_computeShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating light cull shader.", "LightingShader.cpp", MB_OK);
		return false;
	}

	return true;
}