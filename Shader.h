#ifndef _SHADER_H_
#define _SHADER_H_


#define SHADER_RUNTIME_COMPILATION 1


#include "Device.h"


class Shader : public DeviceUser {
public:
	virtual bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0) = 0;
	virtual void SetShader(UINT shaderType = 0) = 0;
};

class VertexShader : public Shader {
protected:
	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11InputLayout> m_layout;

public:
	virtual ~VertexShader() {}

	virtual bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);

	virtual void SetShader(UINT shaderType = 0) {
		m_context->VSSetShader(m_vertexShader.Get(), NULL, 0);
		m_context->IASetInputLayout(m_layout.Get());
	}
};

class GeometryShader : public Shader {
protected:
	ComPtr<ID3D11GeometryShader> m_geometryShader;

public:
	virtual ~GeometryShader() {}

	virtual bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);

	virtual void SetShader(UINT shaderType = 0) {
		m_context->GSSetShader(m_geometryShader.Get(), NULL, 0);
	}
};

class PixelShader : public Shader {
protected:
	ComPtr<ID3D11PixelShader> m_pixelShader;

public:
	virtual ~PixelShader() {}

	virtual bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);

	virtual void SetShader(UINT shaderType = 0) {
		m_context->PSSetShader(m_pixelShader.Get(), NULL, 0);
	}
};

class ComputeShader : public Shader {
protected:
	ComPtr<ID3D11ComputeShader> m_computeShader;

public:
	virtual ~ComputeShader() {}

	virtual bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);

	virtual void SetShader(UINT shaderType = 0) {
		m_context->CSSetShader(m_computeShader.Get(), NULL, 0);
	}
};


class ShaderCompiler {
public:
	static HRESULT CompileFromFile(LPCWSTR filename, LPCSTR entry_point, LPCSTR shader_model, ID3DBlob** blob_out, ID3DBlob** error_message);

	static void OutputErrorMessage(ID3DBlob* error_message, const char* folder_name);
};


#endif