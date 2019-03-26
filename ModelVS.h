#ifndef _MODELVS_H_
#define _MODELVS_H_

#include "Shader.h"


class ModelVS : public VertexShader {
private:
	ComPtr<ID3D11ClassLinkage> m_classLinkage;
	ComPtr<ID3D11ClassInstance> m_staticClass;
	ComPtr<ID3D11ClassInstance> m_skinnedClass;

public:
	bool Initialize();

	void SetShader(UINT shaderType = 0);
};

#endif