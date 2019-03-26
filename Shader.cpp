#pragma comment(lib, "d3dcompiler.lib")

#include <fstream>
#include <string>

#include <d3dcompiler.h>

#include "Shader.h"
using namespace std;

HRESULT ShaderCompiler::CompileFromFile(LPCWSTR filename, LPCSTR entry_point, LPCSTR shader_model, ID3DBlob** blob_out, ID3DBlob** error_message) {
	HRESULT result;

	DWORD shaderFlags = 0;

#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;

	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

#endif

	result = D3DCompileFromFile(filename, NULL, NULL, entry_point, shader_model, shaderFlags, 0, blob_out, error_message);

	return result;
}

void ShaderCompiler::OutputErrorMessage(ID3DBlob* error_message, const char* folder_name) {
	char* compileErrors;
	unsigned long bufferSize, i;
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