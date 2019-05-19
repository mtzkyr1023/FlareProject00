#ifndef _LIGHTINGPASS_H_
#define _LIGHTINGPASS_H_

#define RESULT_TEX 0

#include "RenderPass.h"
#include "Shader.h"

class LightingPass : public RenderPass {
private:
	std::unique_ptr<ComputeShader> m_lightUpdateCS;
	std::unique_ptr<ComputeShader> m_lightingCS;

	ComPtr<ID3D11ShaderResourceView> m_lightSrv;
	ComPtr<ID3D11UnorderedAccessView> m_lightUav;

	ComPtr<ID3D11Buffer> m_viewBuffer;
	ComPtr<ID3D11Buffer> m_projBuffer;

	ComPtr<ID3D11Buffer> m_dirLightBuffer;

private:
	bool InitResultTex();

	bool InitPass();
	bool Rendering();

public:
	LightingPass(UINT width, UINT height) {
		m_width = width;
		m_height = height;
	}
	~LightingPass() {}
};

#endif