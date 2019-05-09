#ifndef _SWAPCHAIN_H_
#define _SWAPCHAIN_H_

#include "Device.h"

class SwapChain : public DeviceUser {
private:
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_rtv;
	ComPtr<ID3D11UnorderedAccessView> m_uav;
	bool m_isVsync;

public:
	SwapChain() {}
	~SwapChain() {}

	//�X���b�v�`�F�C��������
	bool Initialize(HWND hwnd, UINT width, UINT height, UINT msaaCount, bool vsync);

	//�o�b�N�o�b�t�@���T�C�Y
	void ResizeBuffer(int width, int height);

	ID3D11UnorderedAccessView* GetUAV() { return m_uav.Get(); }
	ID3D11RenderTargetView* GetRTV() { return m_rtv.Get(); }

	void EndPass();
};

#endif