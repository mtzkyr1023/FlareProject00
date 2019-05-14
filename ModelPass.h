#ifndef _MODELPASS_H_
#define _MODELPASS_H_


#define COLOR_TEX 0
#define NORMAL_TEX 1
#define DEPTH_TEX 2
#define STENCIL_TEX 3

#define TMP_COLOR_TEX 4
#define TMP_DEPTH_TEX 5

#include <memory>
#include <list>

#include "RenderPass.h"
#include "Shader.h"
#include "MatrixBuffer.h"
#include "Model.h"
#include "Material.h"
#include "Object.h"
#include "Camera.h"

class ModelPass : public RenderPass {
private:
	std::unique_ptr<VertexShader> m_vs;
	std::unique_ptr<PixelShader> m_ps;

	std::unique_ptr<VertexShader> m_watervs;
	std::unique_ptr<GeometryShader> m_watergs;
	std::unique_ptr<PixelShader> m_waterps;

	std::unique_ptr<VertexShader> m_cubemapvs;
	std::unique_ptr<PixelShader> m_cubemapps;

	ComPtr<ID3D11RenderTargetView> m_rtv[VIEW_NUM];
	ComPtr<ID3D11DepthStencilView> m_dsv;

	ComPtr<ID3D11BlendState> m_blendState;

	ComPtr<ID3D11RasterizerState> m_rasterState;
	D3D11_VIEWPORT m_viewport;

	ComPtr<ID3D11Buffer> m_worldBuffer;
	ComPtr<ID3D11Buffer> m_viewBuffer;
	ComPtr<ID3D11Buffer> m_projBuffer;
	ComPtr<ID3D11Buffer> m_vpBuffer;
	ComPtr<ID3D11Buffer> m_materialBuffer;

	ComPtr<ID3D11Buffer> m_shadowBuffer;

	ComPtr<ID3D11ShaderResourceView> m_cubeMapSrv;

	ComPtr<ID3D11SamplerState> m_cubeSampler;
	ComPtr<ID3D11SamplerState> m_wrapSampler;
	ComPtr<ID3D11SamplerState> m_clampSampler;

	std::unique_ptr<ModelObject> m_obj1, m_obj2, m_obj3;
	Camera m_camera;

private:
	bool InitColorTexture();
	bool InitNormalTexture();
	bool InitDepthTexture();
	bool InitRasterState();
	bool InitSamplerState();

public:
	ModelPass(UINT width, UINT height);
	~ModelPass() {}

	bool InitPass();
	bool Rendering();
};

#endif