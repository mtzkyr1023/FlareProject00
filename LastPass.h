#ifndef _LASTPASS_H_
#define _LASTPASS_H_

#include <memory>

#include "RenderPass.h"
#include "SwapChain.h"
#include "Shader.h"

class LastPass : public RenderPass {
private:
	SwapChain m_swapChain;
	HWND m_hwnd;
	bool m_vsync;
	std::unique_ptr<ComputeShader> m_cs;

private:
	bool InitPass();
	bool Rendering();

public:
	LastPass(HWND hwnd, UINT width, UINT height, UINT msaaCount, bool vsync);
	~LastPass() {}
};


#endif