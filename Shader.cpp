#pragma comment(lib, "d3dcompiler.lib")

#include <fstream>
#include <string>

#include <d3dcompiler.h>

#include "Shader.h"
using namespace std;

bool VertexShader::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT res;
	ComPtr<ID3DBlob> VSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(filename, entryPoint, "vs_5_0", VSBlob.GetAddressOf(),
		errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "shader_errors/");
		return false;
	}

	res = m_device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, m_vertexShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating model vs.", "Shader.cpp", MB_OK);
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	res = m_device->CreateInputLayout(layout, ARRAYSIZE(layout), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), m_layout.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating input layout.", "Shader.cpp", MB_OK);
		return false;
	}

	return true;
}

bool PixelShader::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT res;
	ComPtr<ID3DBlob> PSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(filename, entryPoint, "ps_5_0", PSBlob.GetAddressOf(),
		errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "shader_errors/");
		return false;
	}

	res = m_device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, m_pixelShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating ps.", "Shader.cpp", MB_OK);
		return false;
	}

	return true;
}

bool GeometryShader::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT res;
	ComPtr<ID3DBlob> GSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(filename, entryPoint, "gs_5_0", GSBlob.GetAddressOf(),
		errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "shader_errors/");
		return false;
	}

	res = m_device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, m_geometryShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating gs.", "Shader.cpp", MB_OK);
		return false;
	}

	return true;
}


bool ComputeShader::Initialize(LPCWSTR filename, LPCSTR entryPoint) {
	HRESULT res;
	ComPtr<ID3DBlob> CSBlob, errorMessage;

	res = ShaderCompiler::CompileFromFile(filename, entryPoint, "cs_5_0", CSBlob.GetAddressOf(),
		errorMessage.GetAddressOf());
	if (FAILED(res)) {
		ShaderCompiler::OutputErrorMessage(errorMessage.Get(), "shader_errors/");
		return false;
	}

	res = m_device->CreateComputeShader(CSBlob->GetBufferPointer(), CSBlob->GetBufferSize(), NULL, m_computeShader.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating cs.", "Shader.cpp", MB_OK);
		return false;
	}

	return true;
}


HRESULT ShaderCompiler::CompileFromFile(LPCWSTR filename, LPCSTR entry_point, LPCSTR shader_model, ID3DBlob** blob_out, ID3DBlob** error_message) {
	HRESULT result;

	DWORD shaderFlags = 0;

#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;

	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

#endif

	result = D3DCompileFromFile(filename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, shader_model, shaderFlags, 0, blob_out, error_message);

	return result;
}

void ShaderCompiler::OutputErrorMessage(ID3DBlob* error_message, const char* folder_name) {
	char* compileErrors;
	size_t bufferSize, i;
	ofstream ofs;
	string str = "shader/";
	str += folder_name;
	str += "error.txt";

	compileErrors = (char*)(error_message->GetBufferPointer());

	bufferSize = error_message->GetBufferSize();

	ofs.open(str.c_str());

	for (i = 0; i < bufferSize; i++)
		ofs << compileErrors[i];

	ofs.close();

	MessageBox(NULL, "Error compiling shader.  Check shader-error.txt for message.", "shader_error", MB_OK);
}