#include "SwapChain.h"

bool SwapChain::Initialize(HWND hwnd, UINT width, UINT height, UINT msaaCount, bool vsync) {
	HRESULT res;

	ComPtr<IDXGIDevice1> dxgi;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGIFactory> factory;
	DXGI_SWAP_CHAIN_DESC sd;

	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_UNORDERED_ACCESS;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = msaaCount;
	sd.SampleDesc.Quality = 0;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	sd.Windowed = TRUE;

	if (vsync) {
		sd.BufferDesc.RefreshRate.Numerator = 1;
		sd.BufferDesc.RefreshRate.Denominator = 60;
	}
	else {
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;
	}

	m_isVsync = vsync;

	m_device->QueryInterface(__uuidof(IDXGIDevice1*), (void**)dxgi.GetAddressOf());
	dxgi->GetAdapter(adapter.GetAddressOf());
	adapter->GetParent(__uuidof(IDXGIFactory*), (void**)factory.GetAddressOf());

	res = factory->CreateSwapChain(m_device.Get(), &sd, m_swapChain.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creatinf swap chain.", "SwapChain.cpp", MB_OK);
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));

	ID3D11Texture2D* tex;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D*), (void**)&tex);

	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	res = m_device->CreateUnorderedAccessView(tex, &uavDesc, m_uav.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating back buffer uav.", "SwapChain.cpp", MB_OK);
		return false;
	}

	if (tex) {
		tex->Release();
		tex = 0;
	}

	return true;
}

void SwapChain::ResizeBuffer(int width, int height) {

}

void SwapChain::EndPass() {
	if (m_isVsync)
		m_swapChain->Present(1, 0);
	else
		m_swapChain->Present(0, 0);
}