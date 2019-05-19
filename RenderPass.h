#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#define VIEW_NUM 8

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "Device.h"

class RenderPass : public DeviceUser {
protected:
	UINT m_width;
	UINT m_height;
	UINT m_msaaCount;
	UINT m_inputSrvNum;
	UINT m_inputUavNum;
	ID3D11ShaderResourceView* m_inputSrv[VIEW_NUM];
	ID3D11UnorderedAccessView* m_inputUav[VIEW_NUM];
	ComPtr<ID3D11ShaderResourceView> m_outputSrv[VIEW_NUM];
	ComPtr<ID3D11UnorderedAccessView> m_outputUav[VIEW_NUM];

protected:
	virtual bool InitPass() = 0;
	virtual bool Rendering() = 0;

public:
	RenderPass() : m_width(640), m_height(480), m_msaaCount(1), m_inputSrvNum(0), m_inputUavNum(0) {
		for (int i = 0; i < VIEW_NUM; i++) {
			m_inputSrv[i] = NULL;
			m_inputUav[i] = NULL;
		}
	}
	virtual ~RenderPass() {}

	bool Initialize() {
		if (!InitPass())
			return false;

		return true;
	}

	bool Render() {
		if (!Rendering())
			return false;

		m_context->ClearState();

		return true;
	}

	void SetSrv(int num, ID3D11ShaderResourceView* srv) {
		m_inputSrv[num] = srv;
		m_inputSrvNum = num + 1;
	}
	void SetUav(int num, ID3D11UnorderedAccessView* uav) {
		m_inputUav[num] = uav;
		m_inputUavNum = num + 1;
	}

	ID3D11ShaderResourceView* GetSrv(int num) { return m_outputSrv[num].Get(); }
	ID3D11UnorderedAccessView* GetUav(int num) { return m_outputUav[num].Get(); }
};


class PassManager {
private:
	std::unique_ptr<RenderPass> m_lastPass;
	std::unique_ptr<RenderPass> m_lightingPass;
	std::unique_ptr<RenderPass> m_modelPass;
	std::unique_ptr<RenderPass> m_shadowPass;
	std::unique_ptr<RenderPass> m_reflectPass;

public:
	bool Initialize(HWND hwnd, UINT width, UINT height, bool vsync);

	bool Render();
};


#endif