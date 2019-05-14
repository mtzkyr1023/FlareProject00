#include "ModelVS.h"

bool ModelVS::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT res;
	ComPtr<ID3DBlob> VSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(L"shader/model_shader/model_vs.fx", "vs_static_main", "vs_5_0", VSBlob.GetAddressOf(),
		errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "model_shader/");
		return false;
	}

	res = m_device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, m_vertexShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating model vs.", "ModelVS.cpp", MB_OK);
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

	res = ShaderCompiler::CompileFromFile(L"shader/model_shader/model_vs.fx", "vs_skinned_main", "vs_5_0", VSBlob.GetAddressOf(),
		errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "model_shader/");
		return false;
	}

	res = m_device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL,
		m_vertexSkinnedShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating model vs.", "ModelVS.cpp", MB_OK);
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC layout2[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	res = m_device->CreateInputLayout(layout2, ARRAYSIZE(layout2), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(),
		m_skinnedLayout.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating input layout.", "ModelVS.cpp", MB_OK);
		return false;
	}

	return true;
}


void ModelVS::SetShader(UINT shaderType) {
	if (shaderType == 0) {
		m_context->VSSetShader(m_vertexShader.Get(), NULL, 0);
		m_context->IASetInputLayout(m_layout.Get());
	}
	else if (shaderType == 1){
		m_context->VSSetShader(m_vertexSkinnedShader.Get(), NULL, 0);
		m_context->IASetInputLayout(m_skinnedLayout.Get());
	}
}