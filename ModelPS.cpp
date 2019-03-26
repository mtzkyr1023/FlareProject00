#include "ModelPS.h"

bool ModelPS::Initialize() {
	HRESULT res;
	ComPtr<ID3DBlob> PSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(L"shader/model_shader/model_ps.fx", "ps_main", "ps_5_0", PSBlob.GetAddressOf(),
		errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "model_shader/");
		return false;
	}

	res = m_device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, m_pixelShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating model ps.", "ModelPS.cpp", MB_OK);
		return false;
	}

	return true;
}