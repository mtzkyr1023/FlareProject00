#include "LightingPass.h"
#include "LightingShader.h"
#include "LightBuffer.h"
#include "MatrixBuffer.h"
#include "Environment.h"
#include "shader/shader_defines.h"

using namespace std;

bool LightingPass::InitPass() {

	m_lightUpdateCS = make_unique<LightCullShader>();
	m_lightingCS = make_unique<LightingShader>();

	if (!m_lightUpdateCS->Initialize())
		return false;

	if (!m_lightingCS->Initialize())
		return false;

	if (!InitResultTex())
		return false;

	if (!LightBufferFactory::Inst().Initialize())
		return false;

	m_lightSrv = LightBufferFactory::Inst().GetSrv();
	m_lightUav = LightBufferFactory::Inst().GetUav();

	m_viewBuffer = MatrixFactory::Inst().GetViewBuffer();
	m_projBuffer = MatrixFactory::Inst().GetProjBuffer();

	m_dirLightBuffer = Environment::Inst().GetDirLightBuffer();

	return true;
}


bool LightingPass::Rendering() {
	m_lightUpdateCS->SetShader();
	m_context->CSSetUnorderedAccessViews(0, 1, m_lightUav.GetAddressOf(), NULL);

	m_context->CSSetConstantBuffers(0, 1, m_viewBuffer.GetAddressOf());

	m_context->Dispatch(POINT_LIGHT_NUM / 128, 1, 1);

	m_context->ClearState();

	m_lightingCS->SetShader();

	m_inputSrv[m_inputSrvNum++] = m_lightSrv.Get();
	m_context->CSSetShaderResources(0, m_inputSrvNum, m_inputSrv);
	m_context->CSSetUnorderedAccessViews(0, 1, m_outputUav[RESULT_TEX].GetAddressOf(), NULL);

	m_context->CSSetConstantBuffers(0, 1, m_projBuffer.GetAddressOf());

	m_context->Dispatch(32, 32, 1);

	return true;
}


bool LightingPass::InitResultTex() {
	HRESULT res;
	D3D11_TEXTURE2D_DESC texDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	ComPtr<ID3D11Texture2D> tex;

	texDesc.Width = m_width;
	texDesc.Height = m_height;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.MipLevels = 1;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	res = m_device->CreateTexture2D(&texDesc, NULL, tex.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating result tex.", "LightingPass.cpp", MB_OK);
		return false;
	}

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	res = m_device->CreateShaderResourceView(tex.Get(), &srvDesc, m_outputSrv[RESULT_TEX].GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating result srv.", "LightingPass.cpp", MB_OK);
		return false;
	}

	uavDesc.Format = texDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	res = m_device->CreateUnorderedAccessView(tex.Get(), &uavDesc, m_outputUav[RESULT_TEX].GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating result uav.", "LightingPass.cpp", MB_OK);
		return false;
	}

	return true;
}