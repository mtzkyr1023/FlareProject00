
#include "imgui_dx11/imgui.h"
#include "imgui_dx11/imgui_impl_win32.h"
#include "imgui_dx11/imgui_impl_dx11.h"

#include "LastPass.h"
#include "LastCS.h"
using namespace std;


LastPass::LastPass(HWND hwnd, UINT width, UINT height, UINT msaaCount, bool vsync) : m_hwnd(hwnd), m_vsync(vsync) {
	m_width = width;
	m_height = height;
	m_msaaCount = msaaCount;
}

LastPass::~LastPass() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool LastPass::InitPass() {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplWin32_Init(m_hwnd);
	ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());

	ImGui::StyleColorsDark();

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

	ID3D11RenderTargetView* rtvArray[] = {
		m_swapChain.GetRTV(),
	};

	m_context->CSSetShaderResources(0, m_inputSrvNum, m_inputSrv);
	m_context->CSSetUnorderedAccessViews(0, 1, uavArray, NULL);

	m_context->Dispatch(32, 32, 1);

	m_context->ClearState();

	ImGui::Render();
	m_context->OMSetRenderTargets(1, rtvArray, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_swapChain.EndPass();

	return true;
}