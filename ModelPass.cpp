#include <random>


#include "imgui_dx11/imgui.h"
#include "imgui_dx11/imgui_impl_win32.h"
#include "imgui_dx11/imgui_impl_dx11.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_lib/stb_image.h"

#include "ModelPass.h"
#include "ModelVS.h"
#include "ModelPS.h"
#include "WaterShader.h"
#include "CubeMapShader.h"
#include "CopyCS.h"
#include "Environment.h"
#include "Material.h"
using namespace std;
using namespace DirectX;

ModelPass::ModelPass(UINT width, UINT height) {
	m_width = width;
	m_height = height;
}

bool ModelPass::InitPass() {

	m_vs = make_unique<ModelVS>();
	m_ps = make_unique<ModelPS>();
	m_watervs = make_unique<WaterVS>();
	m_watergs = make_unique<WaterGS>();
	m_waterps = make_unique<WaterPS>();
	m_cubemapvs = make_unique<CubeMapVS>();
	m_cubemapps = make_unique<CubeMapPS>();
	m_expvs = make_unique<VertexShader>();
	m_expgs = make_unique<GeometryShader>();
	m_expps = make_unique<PixelShader>();

	if (!m_vs->Initialize())
		return false;

	if (!m_ps->Initialize())
		return false;

	if (!m_watervs->Initialize())
		return false;
	if (!m_watergs->Initialize())
		return false;
	if (!m_waterps->Initialize())
		return false;

	if (!m_cubemapvs->Initialize())
		return false;
	if (!m_cubemapps->Initialize())
		return false;

	if (!m_expvs->Initialize(L"shader/explosion_shader/explosion_vs.fx", "vs_main"))
		return false;
	if (!m_expgs->Initialize(L"shader/explosion_shader/explosion_gs.fx", "gs_main"))
		return false;
	if (!m_expps->Initialize(L"shader/explosion_shader/explosion_ps.fx", "ps_main"))
		return false;

	if (!InitColorTexture())
		return false;

	if (!InitNormalTexture())
		return false;

	if (!InitDepthTexture())
		return false;

	if (!InitRasterState())
		return false;

	if (!InitSamplerState())
		return false;


	if (!MatrixFactory::Inst().Initialize())
		return false;

	if (!Environment::Inst().Initialize())
		return false;

	m_worldBuffer = MatrixFactory::Inst().GetWorldBuffer();
	m_viewBuffer = MatrixFactory::Inst().GetViewBuffer();
	m_projBuffer = MatrixFactory::Inst().GetProjBuffer();
	m_vpBuffer = MatrixFactory::Inst().GetViewProjBuffer();
	m_timeBuffer = MatrixFactory::Inst().GetTimeBuffer();
	m_materialBuffer = Environment::Inst().GetMaterialBuffer();

	m_shadowBuffer = MatrixFactory::Inst().GetShadowBuffer();

	m_dirLightBuffer = Environment::Inst().GetDirLightBuffer();

	ModelFactory::Inst().CreateStaticModel("res/model/monkey_flat.obj", 0);
	ModelFactory::Inst().CreateStaticModel("res/model/sphere.obj", 1);
	ModelFactory::Inst().CreateStaticModel("res/model/Cube_quad.obj", 2);
	ModelFactory::Inst().CreateStaticModel("res/model/water.obj", 3);

	ModelFactory::Inst().CreateBackgroundModel("res/model/CubeMap.obj");

	Environment::Inst().LoadCubeMap("tex/CubeMap01/");

	m_cubeMapSrv = Environment::Inst().GetCubeMap();

	ModelObject::SetManager(&ModelObjectManager::Inst());
	m_obj1 = make_unique<ModelObject>(0);
	m_obj2 = make_unique<ModelObject>(1);
	m_obj3 = make_unique<ModelObject>(2);
	m_obj4 = make_unique<ModelObject>(1);
	m_obj5 = make_unique<ModelObject>(2);

	return true;
}

bool ModelPass::Rendering() {

	float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11RenderTargetView* rtvArray[] = {
		m_rtv[ALBEDO_TEX].Get(),
		m_rtv[NORMAL_TEX].Get(),
		m_rtv[METALNESS_TEX].Get(),
		m_rtv[ROUGHNESS_TEX].Get(),
	};

	for (int i = 0; i < ARRAYSIZE(rtvArray); i++)
		m_context->ClearRenderTargetView(rtvArray[i], color);

	m_context->OMSetRenderTargets(1, m_rtv->GetAddressOf(), NULL);

	m_context->RSSetState(m_rasterState.Get());
	m_context->RSSetViewports(1, &m_viewport);

	shared_ptr<Model> backModel;
	ModelFactory::Inst().GetBackgroundModel(backModel);

	m_cubemapvs->SetShader();
	m_cubemapps->SetShader();

	MatrixFactory::Inst().SetVPMatrix(m_camera.GetViewProjMatrix());

	m_context->VSSetConstantBuffers(0, 1, m_vpBuffer.GetAddressOf());

	m_context->PSSetShaderResources(0, 1, m_cubeMapSrv.GetAddressOf());
	m_context->PSSetSamplers(0, 1, m_wrapSampler.GetAddressOf());

	backModel->SetBuffer(0);
	m_context->DrawIndexed(backModel->GetIndexCount(0), 0, 0);

	m_context->ClearState();

	m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_context->OMSetRenderTargets(ARRAYSIZE(rtvArray), rtvArray, m_dsv.Get());

	m_context->RSSetState(m_rasterState.Get());
	m_context->RSSetViewports(1, &m_viewport);


	MatrixFactory::Inst().SetViewMatrix(m_camera.GetViewMatrix());
	MatrixFactory::Inst().SetProjMatrix(m_camera.GetProjMatrix());

	m_inputSrv[m_inputSrvNum++] = m_cubeMapSrv.Get();

	m_context->PSSetShaderResources(0, m_inputSrvNum, m_inputSrv);
	m_context->PSSetSamplers(0, 1, m_wrapSampler.GetAddressOf());
	m_context->PSSetSamplers(1, 1, m_clampSampler.GetAddressOf());


	list<ModelObject*>& objList = ModelObjectManager::Inst().GetObjectArray();
	vector<shared_ptr<Model>>& modelArray = ModelFactory::Inst().GetModelArray();
	vector<MaterialPBR>& materialArray = MaterialFactory::Inst().GetMaterialArray();
	static float rot1, rot2, timer;
	static bool state;
	rot1 += 0.01f;
	rot2 += 0.0f;
	XMStoreFloat4x4(m_obj1->GetMatrix(), XMMatrixScaling(1.03f, 1.03f, 1.03f) *
		XMMatrixRotationY(rot1) * XMMatrixTranslation(0.0f, 0.1f, 0.0f) * XMMatrixTranslation(0.0f, 1.5f, 0.0f));
//	XMStoreFloat4x4(m_obj1->GetMatrix(), XMMatrixScaling(1.25f, 1.25f, 1.25f) *
//		XMMatrixRotationX(rot1) * XMMatrixRotationY(rot2) *
//		XMMatrixTranslation(0.0f, 1.8f, 0.0f));
	XMStoreFloat4x4(m_obj3->GetMatrix(), XMMatrixScaling(25.0f, 0.01f, 25.0f) * XMMatrixTranslation(0.0f, 0.0, 0.0f));
	XMStoreFloat4x4(m_obj2->GetMatrix(), XMMatrixTranslation(-3.5f, 1.0, 0.0f));
	XMStoreFloat4x4(m_obj4->GetMatrix(), XMMatrixTranslation(3.5f, 1.0, 0.0f));
	XMStoreFloat4x4(m_obj5->GetMatrix(), XMMatrixTranslation(6.5f, 1.0, 0.0f));
//	XMStoreFloat4x4(m_obj3->GetMatrix(), XMMatrixTranslation(3.0f, 1.08f, 0.0f));

	if (GetAsyncKeyState('E')) {
		m_obj1->SetModelStatus(ModelObject::EXPLOSION);
		state = true;
	}
	if (GetAsyncKeyState('R')) {
		m_obj1->SetModelStatus(ModelObject::NORMAL);
		state = false;
	}
	if (state)
		timer++;
	else
		timer = 0;


	static float roughness, metalness;

	MatrixFactory::Inst().CountTimer(timer);

	{
		static float f;
		static int counter;

		ImGui::Begin("PBR_TEST");

		ImGui::SliderFloat("roughness", &roughness, 0.0f, 1.0f);
		ImGui::SliderFloat("metalness", &metalness, 0.0f, 1.0f);

		if (ImGui::Button("Button"))
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	for (auto ite : objList) {
		MatrixFactory::Inst().SetWorldMatrix(ite->GetMatrix());
		const shared_ptr<Model>& tmp = modelArray[ite->GetModelId()];
		if (!tmp)
			continue;

		if (ite->GetStatus() == ModelObject::EXPLOSION) {
			m_expvs->SetShader();
			m_expgs->SetShader();
			m_expps->SetShader();
			m_context->VSSetConstantBuffers(0, 1, m_worldBuffer.GetAddressOf());
			m_context->GSSetConstantBuffers(0, 1, m_viewBuffer.GetAddressOf());
			m_context->GSSetConstantBuffers(1, 1, m_projBuffer.GetAddressOf());
			m_context->GSSetConstantBuffers(2, 1, m_timeBuffer.GetAddressOf());
		}
		else {
			m_vs->SetShader(tmp->GetModelType());
			m_ps->SetShader();
			m_context->GSSetShader(NULL, 0, 0);
			m_context->VSSetConstantBuffers(0, 1, m_viewBuffer.GetAddressOf());
			m_context->VSSetConstantBuffers(1, 1, m_projBuffer.GetAddressOf());
			m_context->VSSetConstantBuffers(2, 1, m_worldBuffer.GetAddressOf());
			m_context->VSSetConstantBuffers(3, 1, m_shadowBuffer.GetAddressOf());
			m_context->PSSetConstantBuffers(1, 1, m_dirLightBuffer.GetAddressOf());
		}
		for (int i = 0; i < modelArray[ite->GetModelId()]->GetMeshCount(); i++) {
/*			Environment::Inst().SetMaterial(
				materialArray[modelArray[ite->GetModelId()]->GetMaterialId()].GetAlbedo(),
				materialArray[modelArray[ite->GetModelId()]->GetMaterialId()].GetMetalness(),
				materialArray[modelArray[ite->GetModelId()]->GetMaterialId()].GetRoughness());*/

			Environment::Inst().SetMaterial(
								materialArray[tmp->GetMaterialId()].GetAlbedo(),
								metalness,
								roughness);
			m_inputSrv[0] = NULL;
			m_context->PSSetShaderResources(0, m_inputSrvNum, m_inputSrv);
			m_context->PSSetConstantBuffers(0, 1, m_materialBuffer.GetAddressOf());
			modelArray[ite->GetModelId()]->SetBuffer(i);
			m_context->DrawIndexed(modelArray[ite->GetModelId()]->GetIndexCount(i), 0, 0);
		}
	}

	m_context->ClearState();

	m_watervs->SetShader();
	m_watergs->SetShader();
	m_waterps->SetShader();


	m_context->OMSetRenderTargets(ARRAYSIZE(rtvArray), rtvArray, m_dsv.Get());
	
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
//	m_context->OMSetBlendState(m_blendState.Get(), blendFactor, 0xffffffff);



	MatrixFactory::Inst().SetWorldMatrix(m_camera.GetInvViewMatrix());
	MatrixFactory::Inst().SetViewMatrix(m_camera.GetViewMatrix());
	MatrixFactory::Inst().SetVPMatrix(m_camera.GetViewProjMatrix());

	Environment::Inst().SetDirectionalLight(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), m_camera.GetViewMatrix());


	m_camera.Run();


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

	res = m_device->CreateRenderTargetView(tex.Get(), &rtvDesc, m_rtv[ALBEDO_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating color rtv.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateShaderResourceView(tex.Get(), &srvDesc, m_outputSrv[ALBEDO_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating color srv.", "ModelPass.cpp", MB_OK);
		return false;
	}

	texDesc.Format = DXGI_FORMAT_R16_FLOAT;

	rtvDesc.Format = texDesc.Format;

	srvDesc.Format = texDesc.Format;

	res = m_device->CreateTexture2D(&texDesc, NULL, tex.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating metalness buffer.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateRenderTargetView(tex.Get(), &rtvDesc, m_rtv[METALNESS_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating metalness rtv.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateShaderResourceView(tex.Get(), &srvDesc, m_outputSrv[METALNESS_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating metalness srv.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateTexture2D(&texDesc, NULL, tex.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating roughness buffer.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateRenderTargetView(tex.Get(), &rtvDesc, m_rtv[ROUGHNESS_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating roughness rtv.", "ModelPass.cpp", MB_OK);
		return false;
	}

	res = m_device->CreateShaderResourceView(tex.Get(), &srvDesc, m_outputSrv[ROUGHNESS_TEX].ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating roughness srv.", "ModelPass.cpp", MB_OK);
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
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
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

bool ModelPass::InitSamplerState() {
	HRESULT res;
	D3D11_SAMPLER_DESC samplerDesc{};
	D3D11_BLEND_DESC blendDesc{};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 1;
	samplerDesc.BorderColor[1] = 1;
	samplerDesc.BorderColor[2] = 1;
	samplerDesc.BorderColor[3] = 1;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	res = m_device->CreateSamplerState(&samplerDesc, m_wrapSampler.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "Sampler State error", "Shader.cpp", MB_OK);
		return false;
	}

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

	res = m_device->CreateSamplerState(&samplerDesc, m_clampSampler.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "Sampler State error", "Shader.cpp", MB_OK);
		return false;
	}

	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	res = m_device->CreateBlendState(&blendDesc, m_blendState.GetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating blending state.", "ModelPass.cpp", MB_OK);
		return false;
	}

	return true;
}

