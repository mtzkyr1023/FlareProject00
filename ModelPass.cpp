#include "ModelPass.h"
#include "ModelVS.h"
#include "ModelPS.h"
using namespace std;


ModelPass::ModelPass(UINT width, UINT height) {
	m_width = width;
	m_height = height;
}

bool ModelPass::InitPass() {

	m_vs = make_unique<ModelVS>();
	m_ps = make_unique<ModelPS>();

	if (!m_vs->Initialize())
		return false;

	if (!m_ps->Initialize())
		return false;

	if (!InitColorTexture())
		return false;

	if (!InitNormalTexture())
		return false;

	if (!InitDepthTexture())
		return false;

	if (!InitRasterState())
		return false;

	if (!MatrixFactory::Inst().Initialize())
		return false;

	m_viewBuffer = MatrixFactory::Inst().GetViewBuffer();
	m_projBuffer = MatrixFactory::Inst().GetProjBuffer();

	return true;
}

bool ModelPass::Rendering() {
	float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	ID3D11RenderTargetView* rtvArray[] = {
		m_rtv[COLOR_TEX].Get(),
		m_rtv[NORMAL_TEX].Get(),
	};

	for (int i = 0; i < ARRAYSIZE(rtvArray); i++)
		m_context->ClearRenderTargetView(rtvArray[i], color);
	m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_context->OMSetRenderTargets(ARRAYSIZE(rtvArray), rtvArray, m_dsv.Get());

	m_vs->SetShader();
	m_ps->SetShader();

	m_context->VSSetConstantBuffers(0, 1, m_viewBuffer.GetAddressOf());
	m_context->VSSetConstantBuffers(1, 1, m_projBuffer.GetAddressOf());


	vector<shared_ptr<Model>>& modelArray = ModelFactory::Inst().GetModelArray();
	for (UINT i = 0; i < modelArray.size(); i++) {
		if (!modelArray[i])
			continue;
		modelArray[i]->SetBuffer();
		m_context->VSSetShaderResources(0, 1, MatrixFactory::Inst().GetWorldView(i).GetAddressOf());
		m_context->DrawIndexedInstanced(modelArray[i]->GetIndexCount(), modelArray[i].use_count() - 1, 0, 0, 0);
	}

	return true;
}

bool ModelPass::InitColorTexture() {
	HRESULT res;
	ComPtr<ID3D11Texture2D> tex;

	D3D11_TEXTURE2D_DESC texDesc{};
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	texDesc.Width = m_width;
	texDesc.Height = m_height;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	res = m_device->CreateTexture2D(&texDesc, NULL, tex.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating color buffer.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateRenderTargetView(tex.Get(), &rtvDesc, m_rtv[COLOR_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating color rtv.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateShaderResourceView(tex.Get(), &srvDesc, m_outputSrv[COLOR_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating color srv.", "ModelPass.cpp", MB_OK);
		return false;
	}

	return true;
}

bool ModelPass::InitNormalTexture() {
	HRESULT res;
	ComPtr<ID3D11Texture2D> tex;

	D3D11_TEXTURE2D_DESC texDesc{};
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	texDesc.Width = m_width;
	texDesc.Height = m_height;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R11G11B10_FLOAT;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	res = m_device->CreateTexture2D(&texDesc, NULL, tex.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating normal buffer.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateRenderTargetView(tex.Get(), &rtvDesc, m_rtv[NORMAL_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating normal rtv.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateShaderResourceView(tex.Get(), &srvDesc, m_outputSrv[NORMAL_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating normal srv.", "ModelPass.cpp", MB_OK);
		return false;
	}

	return true;
}

bool ModelPass::InitDepthTexture() {
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
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	res = m_device->CreateTexture2D(&texDesc, NULL, tex.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating depth stencil buffer.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateDepthStencilView(tex.Get(), &dsvDesc, m_dsv.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating depth stencil view.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateShaderResourceView(tex.Get(), &srvDesc, m_outputSrv[DEPTH_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating depth stencil srv1.", "ModelPass.cpp", MB_OK);
		return false;
	}

	srvDesc.Format = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	res = m_device->CreateShaderResourceView(tex.Get(), &srvDesc, m_outputSrv[STENCIL_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating depth stencil srv2.", "ModelPass.cpp", MB_OK);
		return false;
	}

	return true;
}


bool ModelPass::InitRasterState() {
	HRESULT res;
	D3D11_RASTERIZER_DESC rasterDesc{};

	rasterDesc.CullMode = D3D11_CULL_BACK;
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