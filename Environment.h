#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "Device.h"


class Environment : public DeviceUser {
private:
	ComPtr<ID3D11ShaderResourceView> m_cubeMapSrv;

public:
	bool LoadCubeMap(const char* filename);

	ComPtr<ID3D11ShaderResourceView> GetCubeMap() { return m_cubeMapSrv; }

	static Environment& Inst() {
		static Environment inst;
		return inst;
	}
};


#endif