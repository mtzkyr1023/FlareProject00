#ifndef _REFLECTPASS_H_
#define _REFLECTPASS_H_

#define REFLECT_TEX 0

#include "RenderPass.h"
#include "Shader.h"

class ReflectPass : public RenderPass {
private:
	std::unique_ptr<ComputeShader> m_cs;

	ComPtr<ID3D11ShaderResourceView> m_cubeMapSrv;

private:
	bool InitRelfectMap();

private:
	bool InitPass();
	bool RenderPass();

public:
	ReflectPass(UINT width, UINT height) {
		m_width = width;
		m_height = height;
	}
};


#endif