#ifndef _MODELPASS_H_
#define _MODELPASS_H_


#define COLOR_TEX 0
#define NORMAL_TEX 1
#define DEPTH_TEX 2
#define STENCIL_TEX 3

#include <memory>

#include "RenderPass.h"
#include "Shader.h"
#include "MatrixBuffer.h"
#include "Model.h"

class ModelPass : public RenderPass {
private:
	std::unique_ptr<VertexShader> m_vs;
	std::unique_ptr<PixelShader> m_ps;

	ComPtr<ID3D11RenderTargetView> m_rtv[VIEW_NUM];
	ComPtr<ID3D11DepthStencilView> m_dsv;

	ComPtr<ID3D11RasterizerState> m_rasterState;
	D3D11_VIEWPORT m_viewport;

	ComPtr<ID3D11Buffer> m_viewBuffer;
	ComPtr<ID3D11Buffer> m_projBuffer;

private:
	bool InitColorTexture();
	bool InitNormalTexture();
	bool InitDepthTexture();
	bool InitRasterState();

public:
	ModelPass(UINT width, UINT height);
	~ModelPass() {}

	bool InitPass();
	bool Rendering();
};

#endif