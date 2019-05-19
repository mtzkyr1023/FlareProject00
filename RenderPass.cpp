
#include "imgui_dx11/imgui.h"
#include "imgui_dx11/imgui_impl_win32.h"
#include "imgui_dx11/imgui_impl_dx11.h"


#include "LastPass.h"
#include "ModelPass.h"
#include "LightingPass.h"
#include "ShadowPass.h"
#include "ReflecetPass.h"
using namespace std;

bool PassManager::Initialize(HWND hwnd, UINT width, UINT height, bool vsync) {
	m_lastPass = make_unique<LastPass>(hwnd, width, height, 1, vsync);
	if (!m_lastPass->Initialize())
		return false;

	m_modelPass = make_unique<ModelPass>(width, height);
	if (!m_modelPass->Initialize())
		return false;

	m_lightingPass = make_unique<LightingPass>(width, height);
	if (!m_lightingPass->Initialize())
		return false;

	m_shadowPass = make_unique<ShadowPass>(512, 512);
	if (!m_shadowPass->Initialize())
		return false;

	m_reflectPass = make_unique<ReflectPass>(512, 512);
	if (!m_reflectPass->Initialize())
		return false;

	return true;
}

bool PassManager::Render() {
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (!m_shadowPass->Render())
		return false;

	m_modelPass->SetSrv(SHADOW_TEX, m_shadowPass->GetSrv(SHADOW_TEX));

	if (!m_modelPass->Render())
		return false;

	m_lightingPass->SetSrv(0, m_modelPass->GetSrv(ALBEDO_TEX));
	m_lightingPass->SetSrv(1, m_modelPass->GetSrv(NORMAL_TEX));
	m_lightingPass->SetSrv(2, m_modelPass->GetSrv(DEPTH_TEX));
	m_lightingPass->SetSrv(3, m_modelPass->GetSrv(ROUGHNESS_TEX));
	m_lightingPass->SetSrv(4, m_modelPass->GetSrv(METALNESS_TEX));

	if (!m_lightingPass->Render())
		return false;

	m_reflectPass->SetSrv(ALBEDO_TEX, m_modelPass->GetSrv(ALBEDO_TEX));
	m_reflectPass->SetSrv(NORMAL_TEX, m_modelPass->GetSrv(NORMAL_TEX));
	m_reflectPass->SetSrv(DEPTH_TEX, m_modelPass->GetSrv(DEPTH_TEX));

	if (!m_reflectPass->Render())
		return false;

	m_lastPass->SetSrv(0, m_lightingPass->GetSrv(RESULT_TEX));
	m_lastPass->SetSrv(1, m_reflectPass->GetSrv(REFLECT_TEX));

	if (!m_lastPass->Render())
		return false;

	return true;
}