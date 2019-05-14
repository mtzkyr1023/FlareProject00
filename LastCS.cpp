#include "LastCS.h"

bool LastCS::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT result;
	ComPtr<ID3DBlob> CSBlob, errorMessage;

	result = ShaderCompiler::CompileFromFile(L"shader/compute_shader/last_cs.fx", "cs_main", "cs_5_0", CSBlob.GetAddressOf(), errorMessage.GetAddressOf());
	if (FAILED(result)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "compute_shader/");
		return false;
	}

	result = m_device->CreateComputeShader(CSBlob->GetBufferPointer(), CSBlob->GetBufferSize(), NULL, m_computeShader.GetAddressOf());
	if (FAILED(result)) {
		MessageBox(NULL, "failed creating cs.", "LastCS.cpp", MB_OK);
		return false;
	}

	return true;
}
