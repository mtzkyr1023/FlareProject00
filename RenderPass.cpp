#include "LastPass.h"
#include "ModelPass.h"
#include "LightingPass.h"
#include "ShadowPass.h"
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

	return true;
}

bool PassManager::Render() {
	if (!m_shadowPass->Render())
		return false;

	m_modelPass->SetSrv(SHADOW_TEX, m_shadowPass->GetSrv(SHADOW_TEX));

	if (!m_modelPass->Render())
		return false;

	m_lightingPass->SetSrv(COLOR_TEX, m_modelPass->GetSrv(COLOR_TEX));
	m_lightingPass->SetSrv(NORMAL_TEX, m_modelPass->GetSrv(NORMAL_TEX));
	m_lightingPass->SetSrv(DEPTH_TEX, m_modelPass->GetSrv(DEPTH_TEX));

	if (!m_lightingPass->Render())
		return false;

	m_lastPass->SetSrv(RESULT_TEX, m_lightingPass->GetSrv(RESULT_TEX));

	if (!m_lastPass->Render())
		return false;

	return true;
}