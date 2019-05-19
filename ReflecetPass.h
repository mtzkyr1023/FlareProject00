#ifndef _REFLECTPASS_H_
#define _REFLECTPASS_H_

#define REFLECT_TEX 0

#include "RenderPass.h"
#include "Shader.h"

class ReflectPass : public RenderPass {
private:
	std::unique_ptr<ComputeShader> m_cs;

	ComPtr<ID3D11ShaderResourceView> m_cubeMapSrv;

	ComPtr<ID3D11UnorderedAccessView> m_uav;

	ComPtr<ID3D11SamplerState> m_sampler;

	ComPtr<ID3D11Buffer> m_invViewBuffer;
	ComPtr<ID3D11Buffer> m_projBuffer;
	ComPtr<ID3D11Buffer> m_viewBuffer;
	ComPtr<ID3D11Buffer> m_vpBuffer;

private:
	bool InitRelfectMap();

private:
	bool InitPass();
	bool Rendering();

public:
	ReflectPass(UINT width, UINT height) {
		m_width = width;
		m_height = height;
	}
};


#endif