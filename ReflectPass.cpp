#include "ReflecetPass.h"
#include "Environment.h"
#include "MatrixBuffer.h"
using namespace std;


bool ReflectPass::InitPass() {
	m_cs = make_unique<ComputeShader>();

	if (!m_cs->Initialize(L"shader/compute_shader/ssr_cs.fx", "cs_main"))
		return false;

	if (!InitRelfectMap())
		return false;

	m_invViewBuffer = MatrixFactory::Inst().GetWorldBuffer();
	m_projBuffer = MatrixFactory::Inst().GetProjBuffer();
	m_viewBuffer = MatrixFactory::Inst().GetViewBuffer();
	m_vpBuffer = MatrixFactory::Inst().GetViewProjBuffer();

	m_cubeMapSrv = Environment::Inst().GetCubeMap();

	return true;
}


bool ReflectPass::Rendering() {

	m_inputSrv[m_inputSrvNum] = m_cubeMapSrv.Get();

	m_cs->SetShader();

	m_context->CSSetShaderResources(0, m_inputSrvNum + 1, m_inputSrv);
	m_context->CSSetUnorderedAccessViews(0, 1, m_uav.GetAddressOf(), NULL);

	m_context->CSSetSamplers(0, 1, m_sampler.GetAddressOf());

	m_context->CSSetConstantBuffers(1, 1, m_invViewBuffer.GetAddressOf());
	m_context->CSSetConstantBuffers(0, 1, m_projBuffer.GetAddressOf());
	m_context->CSSetConstantBuffers(2, 1, m_viewBuffer.GetAddressOf());
	m_context->CSSetConstantBuffers(3, 1, m_vpBuffer.GetAddressOf());

	m_context->Dispatch(32, 32, 1);

	return true;
}

bool ReflectPass::InitRelfectMap() {
	HRESULT res;
	D3D11_TEXTURE2D_DESC texDesc{};
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	ComPtr<ID3D11Texture2D> tex;

	texDesc.Width = m_width;
	texDesc.Height = m_height;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	uavDesc.Format = texDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	res = m_device->CreateTexture2D(&texDesc, NULL, tex.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating color buffer.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateUnorderedAccessView(tex.Get(), &uavDesc, m_uav.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating color rtv.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateShaderResourceView(tex.Get(), &srvDesc, m_outputSrv[REFLECT_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating color srv.", "ModelPass.cpp", MB_OK);
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc{};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 1;
	samplerDesc.BorderColor[1] = 1;
	samplerDesc.BorderColor[2] = 1;
	samplerDesc.BorderColor[3] = 1;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	res = m_device->CreateSamplerState(&samplerDesc, m_sampler.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "Sampler State error", "Shader.cpp", MB_OK);
		return false;
	}

	return true;
}