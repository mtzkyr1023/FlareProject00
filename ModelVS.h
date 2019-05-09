#ifndef _MODELVS_H_
#define _MODELVS_H_

#include "Shader.h"


class ModelVS : public VertexShader {
private:
	ComPtr<ID3D11VertexShader> m_vertexSkinnedShader;
	ComPtr<ID3D11InputLayout> m_skinnedLayout;

public:
	bool Initialize();

	void SetShader(UINT shaderType = 0);
};

#endif