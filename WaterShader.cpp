#include "WaterShader.h"


bool WaterVS::Initialize() {
	HRESULT res;
	ComPtr<ID3DBlob> VSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(L"shader/water_shader/water_vs.fx", "vs_main", "vs_5_0",
		VSBlob.GetAddressOf(), errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "shader/water_shader/water_vs_");
		return false;
	}

	res = m_device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, m_vertexShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating water vs.", "WaterShader.cpp", MB_OK);
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	res = m_device->CreateInputLayout(layout, ARRAYSIZE(layout), VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(), m_layout.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating input layuot.", "WaterShader.cpp", MB_OK);
		return false;
	}

	return true;
}


bool WaterGS::Initialize() {
	HRESULT res;
	ComPtr<ID3DBlob> GSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(L"shader/water_shader/water_gs.fx", "gs_main", "gs_5_0", GSBlob.GetAddressOf(),
		errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "water_shader/water_gs_");
		return false;
	}

	res = m_device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL,
		m_geometryShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating water gs.", "WaterShader.cpp", MB_OK);
		return false;
	}

	return true;
}

bool WaterPS::Initialize() {
	HRESULT res;
	ComPtr<ID3DBlob> PSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(L"shader/water_shader/water_ps.fx", "ps_main", "ps_5_0", PSBlob.GetAddressOf(),
		errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "water_shader/water_ps_");
		return false;
	}

	res = m_device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, m_pixelShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating water ps.", "WaterShader.cpp", MB_OK);
		return false;
	}

	return true;
}