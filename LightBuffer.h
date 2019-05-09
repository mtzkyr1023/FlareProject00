#ifndef _LIGHTBUFFER_H_
#define _LIGHTBUFFER_H_

#include <DirectXMath.h>

#include "Device.h"
#include "Light.h"

class LightBufferFactory : public DeviceUser {
private:
	ComPtr<ID3D11Buffer> m_lightBuffer;
	ComPtr<ID3D11ShaderResourceView> m_lightSrv;
	ComPtr<ID3D11UnorderedAccessView> m_lightUav;

public:
	bool Initialize();

	ComPtr<ID3D11Buffer> GetBuffer() { return m_lightBuffer; }
	ComPtr<ID3D11ShaderResourceView> GetSrv() { return m_lightSrv; }
	ComPtr<ID3D11UnorderedAccessView> GetUav() { return m_lightUav; }

	static LightBufferFactory& Inst() {
		static LightBufferFactory inst;
		return inst;
	}
};

#endif