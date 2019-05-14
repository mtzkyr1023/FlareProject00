#include "CopyCS.h"


bool CopyCS::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT res;
	ComPtr<ID3DBlob> CSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(L"shader/compute_shader/copy_cs.fx", "cs_main", "cs_5_0", CSBlob.GetAddressOf(), errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "compute_shader/copy_cs_");
		return false;
	}

	res = m_device->CreateComputeShader(CSBlob->GetBufferPointer(), CSBlob->GetBufferSize(), NULL, m_computeShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating cs.", "CopyCS.cpp", MB_OK);
		return false;
	}

	return true;
}