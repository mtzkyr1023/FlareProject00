
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
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static bool show_demo_window;
	static bool show_another_window;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	{
		static float f;
		static int counter;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

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