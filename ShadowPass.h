#ifndef _SHADOWPASS_H_
#define _SHADOWPASS_H_

#include "RenderPass.h"
#include "Shader.h"

#include "Light.h"

#define SHADOW_TEX 1


class ShadowPass : public RenderPass {
private:
	std::unique_ptr<VertexShader> m_vs;

	ComPtr<ID3D11DepthStencilView> m_dsv;

	ComPtr<ID3D11RasterizerState> m_rasterState;
	D3D11_VIEWPORT m_viewport;

	ComPtr<ID3D11Buffer> m_shadowBuffer;
	ComPtr<ID3D11Buffer> m_worldBuffer;

	DirectionalLight m_light;

private:
	bool InitShadowTexture();
	bool InitRasterState();

	bool InitPass();
	bool Rendering();

public:
	ShadowPass(UINT width, UINT height);
	~ShadowPass() {}
};

#endif