#include "LastPass.h"
#include "ModelPass.h"
using namespace std;

bool PassManager::Initialize(HWND hwnd, UINT width, UINT height, bool vsync) {
	m_lastPass = make_unique<LastPass>(hwnd, width, height, 1, vsync);
	if (!m_lastPass->Initialize())
		return false;

	m_modelPass = make_unique<ModelPass>(width, height);
	if (!m_modelPass->Initialize())
		return false;

	return true;
}

bool PassManager::Render() {
	if (!m_modelPass->Render())
		return false;

	m_lastPass->SetSrv(COLOR_TEX, m_modelPass->GetSrv(COLOR_TEX));
	m_lastPass->SetSrv(NORMAL_TEX, m_modelPass->GetSrv(NORMAL_TEX));

	if (!m_lastPass->Render())
		return false;

	return true;
}