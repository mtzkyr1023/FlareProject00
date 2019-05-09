#include "ShadowPass.h"
#include "ShadowShader.h"
#include "MatrixBuffer.h"
#include "Object.h"
#include "Model.h"
using namespace DirectX;
using namespace std;


ShadowPass::ShadowPass(UINT width, UINT height) {
	m_width = width;
	m_height = height;
}

bool ShadowPass::InitPass() {
	m_vs = make_unique<ShadowVS>();
	if (!m_vs->Initialize())
		return false;

	if (!InitShadowTexture())
		return false;

	if (!InitRasterState())
		return false;

	m_worldBuffer = MatrixFactory::Inst().GetWorldBuffer();
	m_shadowBuffer = MatrixFactory::Inst().GetShadowBuffer();

	return true;
}


bool ShadowPass::Rendering() {
	m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_context->OMSetRenderTargets(0, NULL, m_dsv.Get());

	m_context->RSSetState(m_rasterState.Get());
	m_context->RSSetViewports(1, &m_viewport);

	m_vs->SetShader();

	list<ModelObject*>& objList = ModelObjectManager::Inst().GetObjectArray();
	vector<shared_ptr<Model>>& modelArray = ModelFactory::Inst().GetModelArray();

	MatrixFactory::Inst().SetShadowMatrix(m_light.GetViewMatrix(), m_light.GetProjMatrix());

	m_context->VSSetConstantBuffers(0, 1, m_shadowBuffer.GetAddressOf());

	for (auto ite : objList) {
		MatrixFactory::Inst().SetWorldMatrix(ite->GetMatrix());
		if (!modelArray[ite->GetModelId()])
			continue;

		for (int i = 0; i < modelArray[ite->GetModelId()]->GetMeshCount(); i++) {
			modelArray[ite->GetModelId()]->SetBuffer(i);
			m_context->VSSetConstantBuffers(1, 1, m_worldBuffer.GetAddressOf());
			m_context->DrawIndexed(modelArray[ite->GetModelId()]->GetIndexCount(i), 0, 0);
		}
	}

//	m_light.Run();

	return true;
}


bool ShadowPass::InitShadowTexture() {
	HRESULT res;
	ComPtr<ID3D11Texture2D> tex;
	D3D11_TEXTURE2D_DESC texDesc{};
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	texDesc.Width = m_width;
	texDesc.Height = m_height;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	res = m_device->CreateTexture2D(&texDesc, NULL, tex.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating shadow texture.", "ShadowPass.cpp", MB_OK);
		return false;
	}

	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	res = m_device->CreateDepthStencilView(tex.Get(), &dsvDesc, m_dsv.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating shadow dsv.", "ShadowPass.cpp", MB_OK);
		return false;
	}

	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	res = m_device->CreateShaderResourceView(tex.Get(), &srvDesc, m_outputSrv[SHADOW_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating shadow srv.", "ShadowPass.cpp", MB_OK);
		return false;
	}

	return true;
}

bool ShadowPass::InitRasterState() {
	HRESULT res;
	D3D11_RASTERIZER_DESC rasterDesc{};

	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.DepthBias = 0;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	res = m_device->CreateRasterizerState(&rasterDesc, m_rasterState.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating raster state.", "ModelPass.cpp", MB_OK);
		return false;
	}

	m_viewport.Width = (float)m_width;
	m_viewport.Height = (float)m_height;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.MinDepth = 0.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	return true;
}