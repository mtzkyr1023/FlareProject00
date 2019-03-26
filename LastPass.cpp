#include "LastPass.h"
#include "LastCS.h"
using namespace std;


LastPass::LastPass(HWND hwnd, UINT width, UINT height, UINT msaaCount, bool vsync) : m_hwnd(hwnd), m_vsync(vsync) {
	m_width = width;
	m_height = height;
	m_msaaCount = msaaCount;
}

bool LastPass::InitPass() {

	if (!m_swapChain.Initialize(m_hwnd, m_width, m_height, m_msaaCount, m_vsync))
		return false;

	m_cs = make_unique<LastCS>();
	if (!m_cs->Initialize())
		return false;

	return true;
}

bool LastPass::Rendering() {
	float color[] = { 0.5f, 1.0f, 0.5f, 1.0f };
	m_context->ClearUnorderedAccessViewFloat(m_swapChain.GetUAV(), color);
	m_cs->SetShader();

	ID3D11UnorderedAccessView* uavArray[] = {
		m_swapChain.GetUAV(),
	};

	m_context->CSSetShaderResources(0, m_inputSrvNum, m_inputSrv);
	m_context->CSSetUnorderedAccessViews(0, 1, uavArray, NULL);

	m_context->Dispatch(20, 30, 1);

	m_swapChain.EndPass();

	return true;
}