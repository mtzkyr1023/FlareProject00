#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include <DirectXMath.h>

#include "Device.h"


class Environment : public DeviceUser {
private:
	struct PBR_Type {
		DirectX::XMFLOAT4 albedo;
		float metalness;
		float roughness;
		float padding[2];
	};

	struct DirectionalLightType {
		DirectX::XMFLOAT4 worldSpaceDirection;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 viewSpaceDirection;
	};

private:
	ComPtr<ID3D11ShaderResourceView> m_cubeMapSrv;

	ComPtr<ID3D11Buffer> m_materialBuffer;
	ComPtr<ID3D11Buffer> m_dirLightBuffer;

public:
	bool LoadCubeMap(const char* filename);
	bool Initialize();

	void SetMaterial(DirectX::XMFLOAT4 albedo, float metalness, float roughness);
	void SetDirectionalLight(DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 color, DirectX::XMFLOAT4X4* viewMatrix);

	ComPtr<ID3D11ShaderResourceView> GetCubeMap() { return m_cubeMapSrv; }

	ComPtr<ID3D11Buffer> GetMaterialBuffer() { return m_materialBuffer; }
	ComPtr<ID3D11Buffer> GetDirLightBuffer() { return m_dirLightBuffer; }

	static Environment& Inst() {
		static Environment inst;
		return inst;
	}
};


#endif